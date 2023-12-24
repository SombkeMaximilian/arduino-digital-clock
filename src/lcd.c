// -------------------------------------------------- //
// dependencies

# include <avr/io.h>
# include <avr/interrupt.h>
# include <util/delay.h>

# include "lcd.h"

// ------------------------------------------------------------ //
// macros for bit manipulation of IO registers

# define set_io_bit(port, bit) (port |= (1 << bit))
# define clear_io_bit(port, bit) (port &= ~(1 << bit))
# define change_io_bit(port, bit, value) (value == 1) ? set_io_bit(port, bit) : clear_io_bit(port, bit);


// -------------------------------------------------- //
// configure the LCD pins
// 
// rs = register select (1 = data, 0 = command) 
// rw = read or write (1 = read, 0 = write)
// en = enable pin
// d0-7 = data pins

void config(LCD * lcd, uint8_t rs, uint8_t rw, uint8_t en,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
            uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {
    
    // control pins
    lcd->_rs_pin = rs;
    lcd->_rw_pin = rw;
    lcd->_en_pin = en;
    
    // data bus pins
    lcd->_data_bus[0] = d0;
    lcd->_data_bus[1] = d1;
    lcd->_data_bus[2] = d2;
    lcd->_data_bus[3] = d3;
    lcd->_data_bus[4] = d4;
    lcd->_data_bus[5] = d5;
    lcd->_data_bus[6] = d6;
    lcd->_data_bus[7] = d7;
    
}


// -------------------------------------------------- //
// initialize the LCD
// 
// default settings:
// display on
// cursor off
// cursor blink off
// standard text direction (left to right)
// no display shift
// small characters (5x8dot)

void init(LCD * lcd, uint8_t data_bus_length, uint8_t rows, 
          uint8_t cols) {
    
    // number of lines and columns, and addresses of the first column in each row
    lcd->_rows = rows;
    lcd->_cols = cols;
    lcd->_row_offset[0] = 0x00;
    lcd->_row_offset[1] = 0x40;
    
    // commands without parameters
    lcd->_displaymode     = MASK_ENTRYMODESET;
    lcd->_displaycontrol  = MASK_DISPLAYCONTROL;
    lcd->_displayfunction = MASK_FUNCTIONSET;
    
    // displaymode default settings
    lcd->_displaymode |= FLAG_ENTRY_SHIFTCURSORRIGHT;
    lcd->_displaymode &= FLAG_ENTRY_NOAUTOSHIFT;
    
    // displaycontrol default settings
    lcd->_displaycontrol |= FLAG_DISPLAYCONTROL_DISPLAYON;
    lcd->_displaycontrol &= FLAG_DISPLAYCONTROL_CURSOROFF;
    lcd->_displaycontrol &= FLAG_DISPLAYCONTROL_BLINKOFF;
    
    // 4-bit or 8-bit mode
    switch (data_bus_length) {
    
        case 4:
            
            lcd->_displayfunction &= FLAG_FUNCTIONSET_4BITBUS;
            break;
            
        case 8:
            
            lcd->_displayfunction |= FLAG_FUNCTIONSET_8BITBUS;
            break;
            
    }
    
    // number of lines
    switch (rows) {
    
        case 1:
            
            lcd->_displayfunction &= FLAG_FUNCTIONSET_ONELINE;
            break;
            
        default:
            
            lcd->_displayfunction |= FLAG_FUNCTIONSET_TWOLINE;
            break;
    
    
    }
    
    // displayfunction default settings
    lcd->_displayfunction &= FLAG_FUNCTIONSET_5x8DOT;
    
    // send the commands
    for (int i = 1; i < 3; i++) {
        for (int j = 1; i < 16; j++) {
            setCursorPosition(lcd, i, j);
        }
    }
    
}

// -------------------------------------------------- //
// clear the display and return cursor to position 0

void clearDisplay(LCD * lcd) {
    
    _send(lcd, MASK_CLEARDISPLAY, 0);
    
}


// -------------------------------------------------- //
// return cursor to position 0

void returnHome(LCD * lcd) {
    
    _send(lcd, MASK_RETURNHOME, 0);
    
}


// -------------------------------------------------- //
// text goes from left to right

void textLeftToRight(LCD * lcd) {
    
    lcd->_displaymode |= FLAG_ENTRY_SHIFTCURSORRIGHT;
    _send(lcd, lcd->_displaymode, 0);
    
}


// -------------------------------------------------- //
// text goes from right to left

void textRightToLeft(LCD * lcd) {
    
    lcd->_displaymode &= FLAG_ENTRY_SHIFTCURSORLEFT;
    _send(lcd, lcd->_displaymode, 0);
    
}


// -------------------------------------------------- //
// entire display is shifted on DDRAM write operation
// the shift direction depends on text direction

void autoShiftOn(LCD * lcd) {
    
    lcd->_displaymode |= FLAG_ENTRY_AUTOSHIFT;
    _send(lcd, lcd->_displaymode, 0);
    
}


// -------------------------------------------------- //
// turn off auto shift

void autoShiftOff(LCD * lcd) {
    
    lcd->_displaymode &= FLAG_ENTRY_NOAUTOSHIFT;
    _send(lcd, lcd->_displaymode, 0);
    
}


// -------------------------------------------------- //
// turn display on

void displayOn(LCD * lcd) {
    
    lcd->_displaycontrol |= FLAG_DISPLAYCONTROL_DISPLAYON;
    _send(lcd, lcd->_displaycontrol, 0);
    
}

          
// -------------------------------------------------- //
// turn display off

void displayOff(LCD * lcd) {
    
    lcd->_displaycontrol &= FLAG_DISPLAYCONTROL_DISPLAYOFF;
    _send(lcd, lcd->_displaycontrol, 0);
    
}


// -------------------------------------------------- //
// turn cursor on

void cursorOn(LCD * lcd) {
    
    lcd->_displaycontrol |= FLAG_DISPLAYCONTROL_CURSORON;
    _send(lcd, lcd->_displaycontrol, 0);
    
}


// -------------------------------------------------- //
// turn cursor off

void cursorOff(LCD * lcd) {
    
    lcd->_displaycontrol &= FLAG_DISPLAYCONTROL_CURSOROFF;
    _send(lcd, lcd->_displaycontrol, 0);
    
}


// -------------------------------------------------- //
// turn cursor blink on

void blinkOn(LCD * lcd) {
    
    lcd->_displaycontrol |= FLAG_DISPLAYCONTROL_BLINKON;
    _send(lcd, lcd->_displaycontrol, 0);
    
}


// -------------------------------------------------- //
// turn cursor blink off

void blinkOff(LCD * lcd) {
    
    lcd->_displaycontrol &= FLAG_DISPLAYCONTROL_BLINKOFF;
    _send(lcd, lcd->_displaycontrol, 0);
    
}


// -------------------------------------------------- //
// shift cursor to the left, decrease address counter

void shiftCursorLeft(LCD * lcd) {
    
    _send(lcd, (MASK_DISPLAYCURSORSHIFT & FLAG_DISPLAYCURSORSHIFT_SHIFTCURSOR) & FLAG_DISPLAYCURSORSHIFT_SHIFTLEFT, 0);
    
}


// -------------------------------------------------- //
// shift cursor to the right, increase address counter

void shiftCursorRight(LCD * lcd) {
    
    _send(lcd, (MASK_DISPLAYCURSORSHIFT & FLAG_DISPLAYCURSORSHIFT_SHIFTCURSOR) | FLAG_DISPLAYCURSORSHIFT_SHIFTRIGHT, 0);
    
}


// -------------------------------------------------- //
// shift entire display to the left, cursor moves
// according to the display

void shiftDisplayLeft(LCD * lcd) {
    
    _send(lcd, (MASK_DISPLAYCURSORSHIFT | FLAG_DISPLAYCURSORSHIFT_SHIFTDISPLAY) & FLAG_DISPLAYCURSORSHIFT_SHIFTLEFT, 0);
    
}


// -------------------------------------------------- //
// shift entire display to the right, cursor moves
// according to the display

void shiftDisplayRight(LCD * lcd) {
    
    _send(lcd, (MASK_DISPLAYCURSORSHIFT | FLAG_DISPLAYCURSORSHIFT_SHIFTDISPLAY) | FLAG_DISPLAYCURSORSHIFT_SHIFTRIGHT, 0);
    
}


// -------------------------------------------------- //
// create a custom character

void customCharacter();


// -------------------------------------------------- //
// set the cursor to a position by setting the address 
// counter to the corresponding DDRAM address

void setCursorPosition(LCD * lcd, uint8_t target_row, uint8_t target_col) {
    
    if (target_row < 0) {
        
        target_row = 0;
        
    } else if (target_row >= lcd->_rows) {
        
        target_row = lcd->_rows - 1;
        
    }
    
    if (target_col < 0) {
        
        target_col = 0;
        
    } else if (target_col >= lcd->_cols) {
        
        target_col = lcd->_cols - 1;
        
    }
    
    _send(lcd, MASK_SETDDRAMADDR | (lcd->_row_offset[target_row] + target_col), 0);
    
}


// -------------------------------------------------- //
// sends 1 byte to the LCD

void _send(LCD * lcd, uint8_t message, uint8_t type) {
    
    // set the rs pin 
    switch (type) {
        
        // command
        case 0:
            
            clear_io_bit(PORTB, lcd->_rs_pin);
            
            break;
        
        // data
        case 1:
            
            set_io_bit(PORTB, lcd->_rs_pin);
            
            break;
            
    }
    
    // send the data depending on bus width
    switch (lcd->_displayfunction & FLAG_FUNCTIONSET_8BITBUS) {
        
        // 4 bit bus
        case 0:
            
            // send 4 msb first
            for (int i = 0; i < 4; i++) { 
                change_io_bit(PORTD, lcd->_data_bus[i], ((message >> (i+4)) & 1));
            }
            
            _enablePulse(lcd);
            
            // send 4 lsb last
            for (int i = 0; i < 4; i++) { 
                change_io_bit(PORTD, lcd->_data_bus[i], ((message >> i) & 1));
            }
            
            _enablePulse(lcd);
            
            break;
        
        // 8 bit bus
        default:
            
            for (int i = 0; i < 8; i++) { 
                change_io_bit(PORTD, lcd->_data_bus[i], ((message >> i) & 1));
            }
            
            _enablePulse(lcd);
            
            break;
            
    }
    
}


// -------------------------------------------------- //
// sends a pulse to the enable pin

void _enablePulse(LCD * lcd) {
    
    // 1. pull the pin low and wait for the LCD
    clear_io_bit(PORTB, lcd->_en_pin);
    _delay_us(1);
    
    // 2. pull it high, the enable pulse needs to be >500ns
    set_io_bit(PORTB, lcd->_en_pin);
    //_delay_us(1);
    _delay_ms(500);
    
    // 3. pull it low again and wait for the LCD (>40
    clear_io_bit(PORTB, lcd->_en_pin);
    //_delay_us(50);
    _delay_ms(500);
    
}