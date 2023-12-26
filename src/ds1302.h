# ifndef DS1302_H
# define DS1302_H

// -------------------------------------------------- //
// register addresses (datasheet page 9)

# define REGISTER_SECOND        0x80
# define REGISTER_MINUTE        0x82
# define REGISTER_HOUR          0x84
# define REGISTER_DATE          0x86
# define REGISTER_MONTH         0x88
# define REGISTER_DAY           0x8A
# define REGISTER_YEAR          0x8C
# define REGISTER_WP            0x8E
# define REGISTER_CLOCKBURST    0xBE


// ------------------------------------------------------------ //
// defining months

enum month {
    JAN = 1,
    FEB = 2,
    MAR = 3,
    APR = 4,
    MAY = 5,
    JUN = 6,
    JUL = 7,
    AUG = 8,
    SEP = 9,
    OCT = 10,
    NOV = 11,
    DEC = 12
};


// ------------------------------------------------------------ //
// struct for storing time data

typedef struct DS1302data {
    
    // time
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    
} DS1302data;


// ------------------------------------------------------------ //
// struct for storing information about the pins and settings

typedef struct DS1302 {
    
    // pins
    uint8_t _ce_pin;
    uint8_t _io_pin;
    uint8_t _clk_pin;
    
    // current direction of io pin
    uint8_t _io_dir;
    
} DS1302;


// ------------------------------------------------------------ //
// initialization and configuration of DS1302

void DS1302config(DS1302 * ds1302, uint8_t ce, uint8_t io, uint8_t clk);
void DS1302init(DS1302 * ds1302);


// ------------------------------------------------------------ //
// functions for communicating with the chip

uint8_t _DS1302read(DS1302 * ds1302);
void _DS1302write(DS1302 * ds1302, uint8_t message);
void _DS1302setIOdir(DS1302 * ds1302, uint8_t dir);
void _DS1302setCE(DS1302 * ds1302, uint8_t value);
void _DS1302clockPulse(DS1302 * ds1302);

# endif