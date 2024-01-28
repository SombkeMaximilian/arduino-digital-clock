// -------------------------------------------------- //
// dependencies

# include <stdint.h>

# include <avr/io.h>
# include <util/delay.h>

# include "dht11.h"
# include "macros.h"


// ------------------------------------------------------------ //
// initialization of DHT11

void DHT11init(DHT11 * dht11, uint8_t io) {
    
    // pin
    dht11->_io_pin  = io;
    
    // io direction (initially output)
    dht11->_io_dir  = 1;
    
    // IO pin initially high
    set_io_bit(PORTB, dht11->_io_pin);
    
}


// ------------------------------------------------------------ //
// user command for retrieving temperature and humidity data

void DHT11readData(DHT11 * dht11, DHT11Data * data) {
    
    DHT11beginTransfer(dht11);
    
    // wait for data transfer
    while ((get_io_bit(PINB, dht11->_io_pin) >> dht11->_io_pin) == 1);
    while ((get_io_bit(PINB, dht11->_io_pin) >> dht11->_io_pin) == 0);
    while ((get_io_bit(PINB, dht11->_io_pin) >> dht11->_io_pin) == 1);
    
    // store the data
    data->humi_integral = DHT11read8bit(dht11);
    data->humi_decimal  = DHT11read8bit(dht11);
    data->temp_integral = DHT11read8bit(dht11);
    data->temp_decimal  = DHT11read8bit(dht11);
    data->checksum      = DHT11read8bit(dht11);
    
}


// -------------------------------------------------- //
// reads one byte of data from the DHT11 (datasheet page 7-8)
//
// DHT11 sends data by pulling the io pin high, the value
// depends on the voltage-length
// 70us    = 1
// 26-28us = 0

uint8_t DHT11read8bit(DHT11 * dht11) {
    
    uint8_t buffer = 0;
    
    for (int i = 0; i < 8; i++) {
        
        // wait for the next bit
        while ((get_io_bit(PINB, dht11->_io_pin) >> dht11->_io_pin) == 0);
        
        // wait out a potential 0
        _delay_us(30);
        
        // if the io pin is still high, the data's current bit is 1
        if ((get_io_bit(PINB, dht11->_io_pin) >> dht11->_io_pin) == 1) {
            
            buffer |= (1 << (7 - i));
            
        }
        
        // wait out the remaining high if needed
        while ((get_io_bit(PINB, dht11->_io_pin) >> dht11->_io_pin) == 1);
        
    }
    
    return buffer;
    
}


// -------------------------------------------------- //
// sends the start signal through io pin (datasheet page 6)

void DHT11beginTransfer(DHT11 * dht11) {
    
    // change io pin to output
    DHT11setIOdir(dht11, 1);
    
    // pull the io pin low to begin start signal
    clear_io_bit(PORTB, dht11->_io_pin);
    _delay_ms(20);
    
    // pull the io pin high to wait for the response
    set_io_bit(PORTB, dht11->_io_pin);
    _delay_us(40);
    
    // change io pin to input
    DHT11setIOdir(dht11, 0);
    
}


// -------------------------------------------------- //
// changes the data direction of the io pin
//
// 1 = send to DHT11 (start signal)
// 0 = receive data from DHT11

void DHT11setIOdir(DHT11 * dht11, uint8_t dir) {
    
    dht11->_io_dir = dir;
    change_io_bit(DDRB, dht11->_io_pin, dht11->_io_dir);
    
}