# ifndef DHT11_H
# define DHT11_H

// ------------------------------------------------------------ //
// structs for storing temperature and humidity data

typedef struct DHT11Data {
    
    uint8_t temp_integral;
    uint8_t temp_decimal;
    uint8_t humi_integral;
    uint8_t humi_decimal;
    uint8_t checksum;
    
} DHT11Data;


// ------------------------------------------------------------ //
// struct for storing information about the pins and settings

typedef struct DHT11 {
    
    // io pin
    uint8_t _io_pin;
    
    // current direction of io pin
    uint8_t _io_dir;
    
} DHT11;


// ------------------------------------------------------------ //
// configuration of DHT11

void DHT11config(DHT11 * dht11, uint8_t io);


// ------------------------------------------------------------ //
// user command for retrieving temperature and humidity data

void DHT11readData(DHT11 * dht11, DHT11Data * data);


// ------------------------------------------------------------ //
// functions for communicating with DHT11

void DHT11read8bit(DHT11 * dht11);
void DHT11beginTransfer(DHT11 * dht11);

# endif