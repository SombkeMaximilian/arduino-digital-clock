// -------------------------------------------------- //
// dependencies

# include <stdint.h>
# include <avr/io.h>
# include <util/delay.h>

# include "ds1302.h"
# include "macros.h"


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
    
    // io direction
    ds1302->_io_dir  = 0;
    
}


// -------------------------------------------------- //
// initialize the DS1302

void DS1302init(DS1302 * ds1302) {
    
    // chip enable off and no clock signal (kept low)
    clear_io_bit(PORTB, ds1302->_ce_pin);
    clear_io_bit(PORTB, ds1302->_clk_pin);
    
}


// -------------------------------------------------- //
// reads time data from DS1302

void readTimeData(DS1302data * data) {}


// -------------------------------------------------- //
// writes time data to DS1302

void writeTimeData(DS1302data * data) {}


// -------------------------------------------------- //
// begins communication with the DS1302 with a command
// bit 7 must be high
// bit 6 specifies time data (0) or RAM data (1)
// bit 5-1 specify designated register (addr)
// bit 0 specifies read (1) or write (0) operation

void _DS1302beginCommunication(DS1302 * ds1302, uint8_t addr, uint8_t dir) {
    
    _DS1302setIOdir(ds1302, 1);
    _DS1302setCEpin(ds1302, 1);
    
    uint8_t message = (1 << 7) | ~dir | addr;
    _DS1302write(ds1302, message);
    
    DS1302setIOdir(ds1302, dir);
    
}


// -------------------------------------------------- //
// reads and returns 1 byte from DS1302

uint8_t _DS1302read(DS1302 * ds1302) {
    
    uint8_t buffer = 0;
    
    for (int i = 0; i < 8; i++) {
        
        if ((get_io_bit(PIND, ds1302->_io_pin) >> ds1302->_io_pin) == 1) {
            
            buffer |= (1 << i);
            
        }
        
        _DS1302clockPulse(ds1302);
        
    }
    
    return buffer;
    
}


// -------------------------------------------------- //
// sends 1 byte to DS1302

void _DS1302write(DS1302 * ds1302, uint8_t message) {
    
    for (int i = 0; i < 8; i++) {
        
        change_io_bit(PORTD, ds1302->_io_pin, ((message >> i) & 1));
        _DS1302clockPulse(ds1302);
        
    }
    
}


// -------------------------------------------------- //
// changes the data direction of the io pin
// 1 = write to DS1302
// 0 = read from DS1302

void _DS1302setIOdir(DS1302 * ds1302, uint8_t dir) {
    
    ds1302->_io_dir = dir;
    change_io_bit(DDRD, ds1302->_io_pin, ds1302->_io_dir);
    
}


// -------------------------------------------------- //
// set the CE pin

void _DS1302setCE(DS1302 * ds1302, uint8_t value) {
    
    change_io_bit(PORTB, ds1302->_ce_pin, value);
    
}


// -------------------------------------------------- //
// sends a pulse to the clock pin
// rising edge = initiates write
// falling edge = initiates read

void _DS1302clockPulse(DS1302 * ds1302) {
    
    set_io_bit(PORTB, ds1302->_clk_pin);
    _delay_us(1);
    
    clear_io_bit(PORTB, ds1302->_clk_pin);
    _delay_us(1);
    
}