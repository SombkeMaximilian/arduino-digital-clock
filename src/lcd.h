# ifndef LCD_H
# define LCD_h

// ------------------------------------------------------------ //
// data bus lines/bits

# define DB0 0
# define DB1 1
# define DB2 2
# define DB3 3
# define DB4 4
# define DB5 5
# define DB6 6
# define DB7 7


// ------------------------------------------------------------ //
// masks and flags for the LCD commands (DB7-0)

// command masks
# define MASK_CLEARDISPLAY          (1 << DB0)
# define MASK_RETURNHOME            (1 << DB1)
# define MASK_ENTRYMODESET          (1 << DB2)
# define MASK_DISPLAYCONTROL        (1 << DB3)
# define MASK_DISPLAYCURSORSHIFT    (1 << DB4)
# define MASK_FUNCTIONSET           (1 << DB5)
# define MASK_SETCGRAMADDR          (1 << DB6)
# define MASK_SETDDRAMADDR          (1 << DB7)

// flags for ENTRYMODESET
# define FLAG_ENTRY_SHIFTCURSORRIGHT     (1 << DB1)
# define FLAG_ENTRY_SHIFTCURSORLEFT     ~(1 << DB1)
# define FLAG_ENTRY_AUTOSHIFT            (1 << DB0)
# define FLAG_ENTRY_NOAUTOSHIFT         ~(1 << DB0)

// flags for DISPLAYCONTROL
# define FLAG_DISPLAYCONTROL_DISPLAYON     (1 << DB2)
# define FLAG_DISPLAYCONTROL_DISPLAYOFF   ~(1 << DB2)
# define FLAG_DISPLAYCONTROL_CURSORON      (1 << DB1)
# define FLAG_DISPLAYCONTROL_CURSOROFF    ~(1 << DB1)
# define FLAG_DISPLAYCONTROL_BLINKON       (1 << DB0)
# define FLAG_DISPLAYCONTROL_BLINKOFF     ~(1 << DB0)

// flags for DISPLAYCURSORSHIFT
# define FLAG_DISPLAYCURSORSHIFT_SHIFTDISPLAY    (1 << DB3)
# define FLAG_DISPLAYCURSORSHIFT_SHIFTCURSOR    ~(1 << DB3)
# define FLAG_DISPLAYCURSORSHIFT_SHIFTRIGHT      (1 << DB2)
# define FLAG_DISPLAYCURSORSHIFT_SHIFTLEFT      ~(1 << DB2)

// flags for FUNCTIONSET
# define FLAG_FUNCTIONSET_8BITBUS    (1 << DB4)
# define FLAG_FUNCTIONSET_4BITBUS   ~(1 << DB4)
# define FLAG_FUNCTIONSET_TWOLINE    (1 << DB3)
# define FLAG_FUNCTIONSET_ONELINE   ~(1 << DB3)
# define FLAG_FUNCTIONSET_5x11DOT    (1 << DB2)
# define FLAG_FUNCTIONSET_5x8DOT    ~(1 << DB2)


// ------------------------------------------------------------ //
// struct for storing information about the pins and settings

typedef struct LCD {
    
    // pins
    uint8_t _rs_pin;
    uint8_t _rw_pin;
    uint8_t _en_pin;
    uint8_t _data_bus[8];
    
    // commands to send upon initializing
    uint8_t _displaymode;
    uint8_t _displaycontrol;
    uint8_t _displayfunction;
    
    // display dimensions
    uint8_t _rows;
    uint8_t _cols;
    uint8_t _row_offset[8];
    
} LCD;


// ------------------------------------------------------------ //
// initialization and configuration of the LCD

void config(LCD * lcd, uint8_t rs, uint8_t rw, uint8_t en,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
            uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
void init(LCD * lcd, uint8_t data_bus_length, uint8_t rows, 
          uint8_t cols);


// ------------------------------------------------------------ //
// user commands for interacting with the LCD

void clearDisplay(LCD * lcd);
void returnHome(LCD * lcd);

// entry mode
void textLeftToRight(LCD * lcd);
void textRightToLeft(LCD * lcd);
void autoShiftOn(LCD * lcd);
void autoShiftOff(LCD * lcd);

// display control
void displayOn(LCD * lcd);
void displayOff(LCD * lcd);
void cursorOn(LCD * lcd);
void cursorOff(LCD * lcd);
void blinkOn(LCD * lcd);
void blinkOff(LCD * lcd);

// cursor & display shift
void shiftCursorLeft(LCD * lcd);
void shiftCursorRight(LCD * lcd);
void shiftDisplayLeft(LCD * lcd);
void shiftDisplayRight(LCD * lcd);

// cursor position & custom characters
void customCharacter();
void setCursorPosition(LCD * lcd, uint8_t target_row, uint8_t target_col);

// send commands or data to the LCD
void LCDcommand(LCD * lcd, uint8_t command);
void LCDcharacter(LCD * lcd, uint8_t data);
void LCDprint(LCD * lcd, char * data);

// ------------------------------------------------------------ //
// functions for communicating via the data bus

void _send(LCD * lcd, uint8_t message, uint8_t type);
void _enablePulse(LCD * lcd);

# endif