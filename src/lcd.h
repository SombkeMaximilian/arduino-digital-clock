# ifndef LCD_H
# define LCD_h

// ------------------------------------------------------------ //
// masks and flags for the LCD commands (DB7-0)

# define MSK_CLEARDISPLAY          (1 << 0)
# define MSK_RETURNHOME            (1 << 1)
# define MSK_ENTRYMODESET          (1 << 2)
# define MSK_DISPLAYCONTROL        (1 << 3)
# define MSK_DISPLAYCURSORSHIFT    (1 << 4)
# define MSK_FUNCTIONSET           (1 << 5)
# define MSK_SETCGRAMADDR          (1 << 6)
# define MSK_SETDDRAMADDR          (1 << 7)

// flags for ENTRYMODESET
# define FLAG_ENTRY_SHIFTCURSORRIGHT     (1 << 1)
# define FLAG_ENTRY_SHIFTCURSORLEFT      (0 << 1)
# define FLAG_ENTRY_SHIFTDISPLAYRIGHT    (1 << 0)
# define FLAG_ENTRY_SHIFTDISPLAYLEFT     (0 << 0)

// flags for DISPLAYCONTROL
# define FLAG_DISPLAYCONTROL_ON           (1 << 2)
# define FLAG_DISPLAYCONTROL_OFF          (0 << 2)
# define FLAG_DISPLAYCONTROL_CURSORON     (1 << 1)
# define FLAG_DISPLAYCONTROL_CURSOROFF    (0 << 1)
# define FLAG_DISPLAYCONTROL_BLINKON      (1 << 0)
# define FLAG_DISPLAYCONTROL_BLINKOFF     (0 << 0

// flags for DISPLAYCURSORSHIFT
# define FLAG_DISPLAYCURSORSHIFT_SHIFTDISPLAY    (1 << 3)
# define FLAG_DISPLAYCURSORSHIFT_SHIFTCURSOR     (0 << 3)
# define FLAG_DISPLAYCURSORSHIFT_SHIFTRIGHT      (1 << 2)
# define FLAG_DISPLAYCURSORSHIFT_SHIFTLEFT       (0 << 2)

// flags for FUNCTIONSET
# define FLAG_FUNCTIONSET_8BITBUS    (1 << 4)
# define FLAG_FUNCTIONSET_4BITBUS    (0 << 4)
# define FLAG_FUNCTIONSET_ONELINE    (1 << 3)
# define FLAG_FUNCTIONSET_TWOLINE    (0 << 3)
# define FLAG_FUNCTIONSET_5x11DOT    (1 << 2)
# define FLAG_FUNCTIONSET_5x8DOT     (0 << 2)


// ------------------------------------------------------------ //
// struct for storing information about the pins and settings

typedef struct {
    
    // pins
    uint8_t _rs_pin;
    uint8_t _rw_pin;
    uint8_t _en_pin;
    uint8_t _data_pins[8];
    
} LCD;

# endif