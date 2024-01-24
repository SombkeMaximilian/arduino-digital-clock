// -------------------------------------------------- //
// dependencies

# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>
# include <string.h>

# include <avr/io.h>
# include <util/delay.h>

# include "ds1302.h"
# include "macros.h"


// ------------------------------------------------------------ //
// takes a date and calculates the day of the week from it
// sunday = 1, ..., saturday = 7
// http://www.cadaeic.net/calendar.htm (modified to be 1 indexed)


int DS1302dayOfWeekFromDate(int d, int m, int y) {
    
    return (d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)%7;
    
}


// ------------------------------------------------------------ //
// takes time and date as a format string input and stores it
// in the data struct
// example "Dec 28 2023" and "05:01:20"
// use __TIME__ and __DATE__ to get compile timestamp

void DS1302timeDataInit(timeData * data, const char * date, const char * time) {
    
    static const char * monthStrings[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep",
        "Oct", "Nov", "Dec", NULL
    };
    int hour, minute, second, day, year;
    char monthtext[4];
    
    // parse the strings and extract data
    sscanf(time, "%d:%d:%d", &hour, &minute, &second);
    sscanf(date, "%s %d %d", monthtext, &day, &year);
    
    // fill in the data
    data->second = second;
    data->minute = minute;
    data->hour   = hour;
    data->day    = day;
    data->year   = year-(year/100 * 100);
    
    // translate month names into numbers 1-12
    for (int i = 0; monthStrings[i] != NULL; i++) {
        if (strcmp(monthStrings[i], monthtext) == 0) {
            data->month = i+1;
        }
    }
    
    // get the day of the week
    data->dayofweek = DS1302dayOfWeekFromDate((int) data->day, (int) data->month, (int) data->year);
    
}


// -------------------------------------------------- //
// configure the DS1302 pins
// 
// ce  = chip enable (1 = on, 0 = off)
// io  = io pin for writing and reading data
// clk = clock pin

void DS1302config(DS1302 * ds1302, uint8_t ce, uint8_t io, uint8_t clk) {
    
    // pins
    ds1302->_ce_pin  = ce;
    ds1302->_io_pin  = io;
    ds1302->_clk_pin = clk;
    
    // io direction (initially output)
    ds1302->_io_dir  = 1;
    
}


// -------------------------------------------------- //
// initialize the DS1302

void DS1302init(DS1302 * ds1302) {
    
    // chip enable off and no clock signal (kept low)
    clear_io_bit(PORTB, ds1302->_ce_pin);
    clear_io_bit(PORTB, ds1302->_clk_pin);
    
}


// -------------------------------------------------- //
// stops the clock

void DS1302stopClock(DS1302 * ds1302) {
    
    DS1302setclearFlag(ds1302, 0, FLAG_CLOCKHALT, 1);
    
}


// -------------------------------------------------- //
// starts the clock

void DS1302startClock(DS1302 * ds1302) {
    
    DS1302setclearFlag(ds1302, 0, FLAG_CLOCKHALT, 0);
    
}


// -------------------------------------------------- //
// changes the clock into 12h or 24h mode

void DS1302setClockMode(DS1302 * ds1302, uint8_t mode) {
    
    uint8_t data[8];
    uint8_t currMode;
    uint8_t hour;
    uint8_t ampm;
    
    // read the data in burst mode
    DS1302beginCommunication(ds1302, REGISTER_CLOCKBURST, 0);
    
    for (int i = 0; i < 8; i++) {
        
        data[i] = DS1302read(ds1302);
        
    }
    
    DS1302setCEpin(ds1302, 0);
    
    // current mode
    currMode = (data[2] >> 7);
    
    // no need to do anything if the desired mode is already active
    if (currMode != mode) {
    
        // set/clear the 12/24h flag and convert between 12h and 24h
        switch (mode) {

            case 1:

                // extract the hour data (assume AM by default)
                hour = bcd_to_dec(data[2] & MASK_HOUR);
                ampm = 0;

                // convert to 12h format if necessary
                if (hour > 12) {

                    hour -= 12;
                    ampm = 1;

                }

                // set the hour data again, including AM / PM bit
                data[2] = (dec_to_bcd(hour)) | (ampm << 5);

                // set the 12h mode flag
                data[2] |= FLAG_12HOURMODE;
                break;

            case 0:

                // extract the hour data
                hour = data[2] & MASK_HOUR;

                // convert to 24h format if necessary (check if PM)
                if ((hour >> 5) == 1) {

                    // bit 4-0 contain the hour data without the AM / PM bit
                    hour &= MASK_HOURNOAMPM;
                    hour = bcd_to_dec(hour) + 12;
                    data[2] = dec_to_bcd(hour);

                }

                // set the 24h mode (clear 12h mode flag)
                data[2] &= FLAG_24HOURMODE;
                break;

        }

        // set the write protect flag in the read data
        data[7] = FLAG_WRITEPROTECT;

        // clear write protection flag in the ds1302
        DS1302beginCommunication(ds1302, REGISTER_WP, 1);
        DS1302write(ds1302, 0);
        DS1302setCEpin(ds1302, 0);

        // write the data back (including set write protect flag)
        DS1302beginCommunication(ds1302, REGISTER_CLOCKBURST, 1);

        for (int i = 0; i < 7; i++) {

            DS1302write(ds1302, data[i]);

        }

        DS1302setCEpin(ds1302, 0);
    
    }
    
}


// -------------------------------------------------- //
// reads time data from DS1302
// internally the data is in bcd format so we need to
// convert it to decimal
// uses burst mode (datasheet page 8)

void DS1302readTimeData(DS1302 * ds1302, timeData * data) {
    
    // begin communication in burst mode
    DS1302beginCommunication(ds1302, REGISTER_CLOCKBURST, 0);
    
    // fetch the data
    data->second    = DS1302read(ds1302) & MASK_SECOND;
    data->minute    = DS1302read(ds1302) & MASK_MINUTE;
    data->hour      = DS1302read(ds1302) & MASK_HOUR;
    data->day       = DS1302read(ds1302) & MASK_DAY;
    data->month     = DS1302read(ds1302) & MASK_MONTH;
    data->dayofweek = DS1302read(ds1302) & MASK_DAYOFWEEK;
    data->year      = DS1302read(ds1302);
    
    // end communication
    DS1302setCEpin(ds1302, 0);
    
}


// -------------------------------------------------- //
// writes time data to DS1302
// data in decimal needs to be converted to bcd format
// uses burst mode (datasheet page 8)

void DS1302writeTimeData(DS1302 * ds1302, timeData * data) {
    
    // clear write protection flag
    DS1302beginCommunication(ds1302, REGISTER_WP, 1);
    DS1302write(ds1302, 0);
    DS1302setCEpin(ds1302, 0);
    
    // begin communication in burst mode
    DS1302beginCommunication(ds1302, REGISTER_CLOCKBURST, 1);
    
    // write the data
    DS1302write(ds1302, dec_to_bcd(data->second));
    DS1302write(ds1302, dec_to_bcd(data->minute));
    DS1302write(ds1302, dec_to_bcd(data->hour));
    DS1302write(ds1302, dec_to_bcd(data->day));
    DS1302write(ds1302, dec_to_bcd(data->month));
    DS1302write(ds1302, dec_to_bcd(data->dayofweek));
    DS1302write(ds1302, dec_to_bcd(data->year));
    
    // set write protection flag again
    DS1302write(ds1302, FLAG_WRITEPROTECT);
    
    // end communication
    DS1302setCEpin(ds1302, 0);
    
}


// -------------------------------------------------- //
// begins communication with the DS1302 with a command
// bit 7 must be high
// bit 6 specifies time data (0) or RAM data (1)
// bit 5-1 specify designated register (addr)
// bit 0 specifies read (1) or write (0) operation

void DS1302beginCommunication(DS1302 * ds1302, uint8_t addr, uint8_t dir) {
    
    DS1302setIOdir(ds1302, 1);
    DS1302setCEpin(ds1302, 1);
    
    uint8_t message = (1 << 7) | ~dir | addr;
    DS1302write(ds1302, message);
    
    DS1302setIOdir(ds1302, dir);
    
}


// -------------------------------------------------- //
// reads and returns 1 byte from DS1302

uint8_t DS1302read(DS1302 * ds1302) {
    
    uint8_t buffer = 0;
    
    for (int i = 0; i < 8; i++) {
        
        if ((get_io_bit(PIND, ds1302->_io_pin) >> ds1302->_io_pin) == 1) {
            
            buffer |= (1 << i);
            
        }
        
        DS1302clockPulse(ds1302);
        
    }
    
    return buffer;
    
}


// -------------------------------------------------- //
// sends 1 byte to DS1302

void DS1302write(DS1302 * ds1302, uint8_t message) {
    
    for (int i = 0; i < 8; i++) {
        
        change_io_bit(PORTD, ds1302->_io_pin, ((message >> i) & 1));
        DS1302clockPulse(ds1302);
        
    }
    
}


// -------------------------------------------------- //
// set or clear a flag (only used for clock halt for now)

void DS1302setclearFlag(DS1302 * ds1302, uint8_t flag_register, uint8_t flag, uint8_t setclear) {
    
    uint8_t data[8];
    
    // read the data in burst mode
    DS1302beginCommunication(ds1302, REGISTER_CLOCKBURST, 0);
    
    for (int i = 0; i < 8; i++) {
        
        data[i] = DS1302read(ds1302);
        
    }
    
    DS1302setCEpin(ds1302, 0);
    
    // set (setclear = 1) or clear (setclear = 0) the flag
    switch (setclear) {
        
        case 1:
            
            data[flag_register] |= flag;
            break;
        
        case 0:
        
            data[flag_register] &= ~flag;
            break;
        
    }
    
    // set the write protect flag in the read data
    data[7] = FLAG_WRITEPROTECT;
    
    // clear write protection flag in the ds1302
    DS1302beginCommunication(ds1302, REGISTER_WP, 1);
    DS1302write(ds1302, 0);
    DS1302setCEpin(ds1302, 0);
    
    // write the data back (including set write protect flag)
    DS1302beginCommunication(ds1302, REGISTER_CLOCKBURST, 1);
    
    for (int i = 0; i < 7; i++) {
        
        DS1302write(ds1302, data[i]);
        
    }
    
    DS1302setCEpin(ds1302, 0);
    
}


// -------------------------------------------------- //
// changes the data direction of the io pin
// 1 = write to DS1302
// 0 = read from DS1302

void DS1302setIOdir(DS1302 * ds1302, uint8_t dir) {
    
    ds1302->_io_dir = dir;
    change_io_bit(DDRD, ds1302->_io_pin, ds1302->_io_dir);
    
}


// -------------------------------------------------- //
// set the CE pin

void DS1302setCEpin(DS1302 * ds1302, uint8_t value) {
    
    change_io_bit(PORTB, ds1302->_ce_pin, value);
    
}


// -------------------------------------------------- //
// sends a pulse to the clock pin
// rising edge = initiates write
// falling edge = initiates read

void DS1302clockPulse(DS1302 * ds1302) {
    
    set_io_bit(PORTB, ds1302->_clk_pin);
    _delay_us(1);
    
    clear_io_bit(PORTB, ds1302->_clk_pin);
    _delay_us(1);
    
}