// ------------------------------------------------------------ //
// dependencies

# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>
# include <string.h>

# include <avr/io.h>
# include <avr/interrupt.h>
# include <util/delay.h>

# include "lcd.h"
# include "ds1302.h"
# include "dht11.h"
# include "macros.h"


// ------------------------------------------------------------ //
// constants

# ifndef F_CPU
# define F_CPU 16000000UL
# endif


// ------------------------------------------------------------ //
// function that formats the time into a string

char * formatTime(timeData * data) {
    
    char * time = malloc(17 * sizeof(*time));
    
    sprintf(time, "%02d:%02d:%02d", bcd_to_dec(data->hour), bcd_to_dec(data->minute), bcd_to_dec(data->second));
    
    return time;
    
}


// ------------------------------------------------------------ //
// function that formats the date into a string

char * formatDate(timeData * data) {
    
    char * date = malloc(17 * sizeof(*date));
    char day[4];
    
    switch (data->dayofweek) {
        case 0:
            strcpy(day, "SUN");
            break;
        case 1:
            strcpy(day, "MON");
            break;
        case 2:
            strcpy(day, "TUE");
            break;
        case 3:
            strcpy(day, "WED");
            break;
        case 4:
            strcpy(day, "THU");
            break;
        case 5:
            strcpy(day, "FRI");
            break;
        case 6:
            strcpy(day, "SAT");
            break;
    }
    
    sprintf(date, "%s %02d.%02d.20%02d", day, bcd_to_dec(data->day), bcd_to_dec(data->month), bcd_to_dec(data->year));
    
    return date;
    
}


// ------------------------------------------------------------ //
// digital clock mode 
// 0 = display time & date
// 1 = humidity & temperature
// 2 = auto-scroll text
uint8_t mode = 0;


// ------------------------------------------------------------ //
// main

int main (void) {
    
    LCD lcd;
    DS1302 ds1302;
    timeData curr_date_time;
    DHT11Data curr_humi_temp;
    char * time;
    char * date;
    char * scrolling_text;
    uint8_t reinit_time;
    
    // configure interrupts for the button to switch mode
    DDRD  = (0 << PD2);
    PORTD = (1 << PD2);
    EICRA = (1 << ISC01) | (0 << ISC00);
    EIMSK = (1 << INT0);
    sei();
    
    // flag for setting the time again
    reinit_time = 0;
    
    // set pins to output
    DDRD = (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);
    DDRB = (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5);
    
    // initialize the RTC
    DS1302init(&ds1302, PB4, PD7, PB5);
    
    // time and date should only be initialized once (RTC takes care of it afterwards)
    if (reinit_time == 1) {

        // get compile time and send it to the RTC module (maybe add an offset later)
        DS1302timeDataInit(&curr_date_time, __DATE__, __TIME__, 4);
        DS1302writeTimeData(&ds1302, &curr_date_time);
        
        // 24h mode
        DS1302setClockMode(&ds1302, 0);
        
        // start the clock
        DS1302startClock(&ds1302);
        
    }
    
    // configure and initialize the LCD
    LCDconfig(&lcd, PB1, 0xFF, PB2, PD3, PD4, PD5, PD6, 0, 0, 0, 0);
    LCDinit(&lcd, 4, 2, 16, 1);
    
    // master loop
    while (1) {
        
        switch (mode) {
    
            case 0:

                // loop that displays the clock
                while (1) {

                    // read the data
                    DS1302readTimeData(&ds1302, &curr_date_time);

                    // format it
                    time = formatTime(&curr_date_time);
                    date = formatDate(&curr_date_time);

                    // return to position (0,0) and print the data
                    LCDreturnHome(&lcd);
                    LCDprint(&lcd, time);
                    LCDsetCursorPosition(&lcd, 1, 0);
                    LCDprint(&lcd, date);

                    // free the memory
                    free(time);
                    free(date);
                    
                    // check if still in clock mode
                    if (mode != 0) {

                        LCDclearDisplay(&lcd);
                        break;

                    }

                }

                break;

            case 1:

                // loop that displays temperature and humidity (will maybe add this with a DHT11)
                while (1) {

                    LCDreturnHome(&lcd);
                    LCDprint(&lcd, "not implemented");
                    
                    // check if still in temperature and humidity mode
                    if (mode != 1) {

                        LCDclearDisplay(&lcd);
                        break;

                    }

                }

                break;

            case 2:

                // loop that displays some automatically scrolling text
                while (1) {
                    
                    // 40 characters can fit in one line (null terminator is filtered out)
                    scrolling_text = malloc(41 * sizeof(*scrolling_text));
                    strcpy(scrolling_text, "this is some auto-scrolling text!");
                    
                    // print the whole line, excess of 16 characters is in memory
                    LCDreturnHome(&lcd);
                    LCDprint(&lcd, scrolling_text);
                    
                    // scroll the display
                    for (int i = 0; i < strlen(scrolling_text) - 16; i++) {
                        
                        _delay_ms(1000);
                        LCDshiftDisplayLeft(&lcd);
                        
                        // to avoid getting stuck here
                        if (mode != 2) {
                            
                            break;
                            
                        }
                        
                    }
                    
                    _delay_ms(2000);
                    
                    free(scrolling_text);
                    
                    // check if still in auto-scroll text mode
                    if (mode != 2) {

                        LCDclearDisplay(&lcd);
                        break;

                    }
                    
                }

                break;

        }
        
    }
    
    return 0;
    
}


// ------------------------------------------------------------ //
// interrupt service routine for INT0
// cycle modes of the digital clock

ISR(INT0_vect) {
    
    mode ++;
    
    if (mode > 2) {
        
        mode = 0;
        
    }
    
}