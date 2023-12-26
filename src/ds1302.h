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


// -------------------------------------------------- //
// register data extraction masks (datasheet page 9)

# define MASK_SECOND      ~(1 << 7)
# define MASK_MINUTE      ~(1 << 7)
# define MASK_HOUR        ~((1 << 7) | (1 << 6))
# define MASK_DAY         ~((1 << 7) | (1 << 6))
# define MASK_MONTH       ~((1 << 7) | (1 << 6) | (1 << 5))
# define MASK_DAYOFWEEK    ((1 << 2) | (1 << 1) | (1 << 0))


// -------------------------------------------------- //
// flags

# define FLAG_CLOCKHALT       (1 << 7)
# define FLAG_WRITEPROTECT    (1 << 7)


// ------------------------------------------------------------ //
// defining months and days

enum daysofweek {
    MON = 1,
    TUE = 2,
    WED = 3,
    THU = 4,
    FRI = 5,
    SAT = 6,
    SUN = 7
};

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
    
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t dayofweek;
    uint8_t year;
    
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
// function for getting time and date from system

void generateDS1302data(DS1302data * data);


// ------------------------------------------------------------ //
// initialization and configuration of DS1302

void DS1302config(DS1302 * ds1302, uint8_t ce, uint8_t io, uint8_t clk);
void DS1302init(DS1302 * ds1302);


// ------------------------------------------------------------ //
// user commands for interacting with DS1302

void readTimeData(DS1302 * ds1302, DS1302data * data);
void writeTimeData(DS1302 * ds1302, DS1302data * data);


// ------------------------------------------------------------ //
// functions for communicating with DS1302

void _DS1302beginCommunication(DS1302 * ds1302, uint8_t addr, uint8_t dir);
uint8_t _DS1302read(DS1302 * ds1302);
void _DS1302write(DS1302 * ds1302, uint8_t message);
void _DS1302setIOdir(DS1302 * ds1302, uint8_t dir);
void _DS1302setCEpin(DS1302 * ds1302, uint8_t value);
void _DS1302clockPulse(DS1302 * ds1302);

# endif