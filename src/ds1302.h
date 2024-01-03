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
# define FLAG_12HOURMODE      (1 << 7)
# define FLAG_24HOURMODE     ~(1 << 7)


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

typedef struct timeData {
    
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t dayofweek;
    uint8_t year;
    
} timeData;


// ------------------------------------------------------------ //
// struct for storing information about the pins and settings

typedef struct DS1302 {
    
    // pins
    uint8_t _ce_pin;
    uint8_t _io_pin;
    uint8_t _clk_pin;
    
    // current direction of io pin
    uint8_t _io_dir;
    
    // clock mode (1 = 12h, 0 = 24h)
    uint8_t _clockmode;
    
} DS1302;


// ------------------------------------------------------------ //
// functions for generating and storing the time data

int DS1302DayOfWeekFromDate(int d, int m, int y);
void DS1302TimeDataInit(timeData * data, const char * date, const char * time);

// ------------------------------------------------------------ //
// initialization and configuration of DS1302

void DS1302config(DS1302 * ds1302, uint8_t ce, uint8_t io, uint8_t clk);
void DS1302init(DS1302 * ds1302);


// ------------------------------------------------------------ //
// user commands for interacting with DS1302

void DS1302readTimeData(DS1302 * ds1302, timeData * data);
void DS1302writeTimeData(DS1302 * ds1302, timeData * data);
void DS1302setClockMode(DS1302 * ds1302, uint8_t mode);


// ------------------------------------------------------------ //
// functions for communicating with DS1302

void DS1302beginCommunication(DS1302 * ds1302, uint8_t addr, uint8_t dir);
uint8_t DS1302read(DS1302 * ds1302);
void DS1302write(DS1302 * ds1302, uint8_t message);
void DS1302setIOdir(DS1302 * ds1302, uint8_t dir);
void DS1302setCEpin(DS1302 * ds1302, uint8_t value);
void DS1302clockPulse(DS1302 * ds1302);

# endif