// -------------------------------------------------- //
// dependencies

# include <stdint.h>

# include <avr/io.h>
# include <util/delay.h>

# include "dht11.h"
# include "macros.h"


// ------------------------------------------------------------ //
// configuration of DHT11

void DHT11config(DHT11 * dht11, uint8_t io) {
    
    // pin
    dht11->_io_pin  = io;
    
    // io direction (initially output)
    dht11->_io_dir  = 1;
    
}


// ------------------------------------------------------------ //
// user command for retrieving temperature and humidity data

void DHT11readData(DHT11 * dht11, DHT11Data * data) {
    
    
    
    
    
    
}


// -------------------------------------------------- //
// reads one byte of data from the DHT11

void DHT11read(DHT11 * dht11) {
    
    
    
    
    
}


// -------------------------------------------------- //
// sends the start signal through io pin (datasheet page 6)

void DHT11beginTransfer(DHT11 * dht11) {
    
    
    
    
    
    
}