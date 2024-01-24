// -------------------------------------------------- //
// dependencies

# include <stdint.h>
# include <avr/io.h>
# include <avr/interrupt.h>
# include <util/delay.h>

# include "lcd.h"
# include "macros.h"


// -------------------------------------------------- //
// configure the LCD pins
// 
// rs = register select (1 = data, 0 = command) 
// rw = read or write (1 = read, 0 = write)
// en = enable pin
// d0-7 = data pins
// if rw is not to be set, enter 0b11111111 (0xFF)

void LCDconfig(LCD * lcd, uint8_t rs, uint8_t rw, uint8_t en,
               uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
               uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {
    
    // control pins
    lcd->_rs_pin = rs;
    lcd->_rw_pin = rw;
    lcd->_en_pin = en;
    
    // timer2 pwm pin
    lcd->_v0_pin = PB4;
    
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

void LCDinit(LCD * lcd, uint8_t data_bus_length, uint8_t rows, 
             uint8_t cols, uint8_t pwm_contrast) {
    
    // number of lines and columns, and addresses of the first column in each row
    lcd->_rows = rows;
    lcd->_cols = cols;
    lcd->_row_offset[0] = 0x00;
    lcd->_row_offset[1] = 0x40;
    
    // commands without parameters
    lcd->_entrymode       = MASK_ENTRYMODESET;
    lcd->_displaycontrol  = MASK_DISPLAYCONTROL;
    lcd->_displayfunction = MASK_FUNCTIONSET;
    
    // displaymode default settings
    lcd->_entrymode |= FLAG_ENTRY_SHIFTCURSORRIGHT;
    lcd->_entrymode &= FLAG_ENTRY_NOAUTOSHIFT;
    
    // displaycontrol default settings
    lcd->_displaycontrol |= FLAG_DISPLAYCONTROL_DISPLAYON;
    lcd->_displaycontrol &= FLAG_DISPLAYCONTROL_CURSOROFF;
    lcd->_displaycontrol &= FLAG_DISPLAYCONTROL_BLINKOFF;
    
    // 4-bit or 8-bit mode
    switch (data_bus_length) {
    
        case 4:
            
            lcd->_displayfunction &= FLAG_FUNCTIONSET_4BITBUS;
            break;
            
        default:
            
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
    
    // initialize according to the datasheet (page 45-46)
    // first wait for more than 40ms
    _delay_ms(50);
    
    // pull rs and en pins low, also rw pin if applicable
    clear_io_bit(PORTB, lcd->_en_pin);
    clear_io_bit(PORTB, lcd->_rs_pin);
    if (lcd->_rw_pin != 0xFF) {clear_io_bit(PORTB, lcd->_rw_pin);}
    
    // enter 4- or 8-bit mode
    switch (data_bus_length) {
    
        case 4:
            
            // for 4-bit mode, send the 4 msb of the command to enter 8-bit mode 3 times
            LCDsend4bit(lcd, ((MASK_FUNCTIONSET | FLAG_FUNCTIONSET_8BITBUS) >> 4));
            _delay_ms(5);
            LCDsend4bit(lcd, ((MASK_FUNCTIONSET | FLAG_FUNCTIONSET_8BITBUS) >> 4));
            _delay_ms(5);
            LCDsend4bit(lcd, ((MASK_FUNCTIONSET | FLAG_FUNCTIONSET_8BITBUS) >> 4));
            _delay_us(200);
            
            // then send the command to enter 4-bit mode
            LCDsend4bit(lcd, ((MASK_FUNCTIONSET & FLAG_FUNCTIONSET_4BITBUS) >> 4));
            
            break;
            
        default:
            
            // for 8-bit mode, send the command to enter 8-bit mode 3 times
            LCDsend8bit(lcd, (MASK_FUNCTIONSET | FLAG_FUNCTIONSET_8BITBUS));
            _delay_ms(5);
            LCDsend8bit(lcd, (MASK_FUNCTIONSET | FLAG_FUNCTIONSET_8BITBUS));
            _delay_us(200);
            LCDsend8bit(lcd, (MASK_FUNCTIONSET | FLAG_FUNCTIONSET_8BITBUS));
            
            break;
            
    }
    
    // 4- or 8-bit mode cannot be changed anymore, set the remaining settings
    LCDcommand(lcd, lcd->_displayfunction);
    LCDcommand(lcd, lcd->_displaycontrol);
    LCDclearDisplay(lcd);
    LCDcommand(lcd, lcd->_entrymode);
    
    // send pwm signal via timer2 to control contrast if flag is set (pin 11 / B4)
    if (pwm_contrast == 1) {
        
        // initialize timer2 in non-inverting fast pwm mode
        set_io_bit(DDRB, PB3);
        TCCR2A = (1 << COM2A1) | (1 << WGM21) | (1 << WGM20);
        TCCR2B = (1 << CS20);
        OCR2A  = 85;
        
    }
    
}


// -------------------------------------------------- //
// uses the Arduino's timer2 to control the contrast

void LCDpwmSetContrast(LCD * lcd, uint8_t contrast) {
    
    OCR2A = contrast;
    
}


// -------------------------------------------------- //
// clear the display and return cursor to position 0

void LCDclearDisplay(LCD * lcd) {
    
    LCDcommand(lcd, MASK_CLEARDISPLAY);
    _delay_ms(2);
    
}


// -------------------------------------------------- //
// return cursor to position 0

void LCDreturnHome(LCD * lcd) {
    
    LCDcommand(lcd, MASK_RETURNHOME);
    _delay_ms(2);
    
}


// -------------------------------------------------- //
// text goes from left to right

void LCDtextLeftToRight(LCD * lcd) {
    
    lcd->_entrymode |= FLAG_ENTRY_SHIFTCURSORRIGHT;
    LCDcommand(lcd, lcd->_entrymode);
    
}


// -------------------------------------------------- //
// text goes from right to left

void LCDtextRightToLeft(LCD * lcd) {
    
    lcd->_entrymode &= FLAG_ENTRY_SHIFTCURSORLEFT;
    LCDcommand(lcd, lcd->_entrymode);
    
}


// -------------------------------------------------- //
// entire display is shifted on DDRAM write operation
// the shift direction depends on text direction

void LCDautoShiftOn(LCD * lcd) {
    
    lcd->_entrymode |= FLAG_ENTRY_AUTOSHIFT;
    LCDcommand(lcd, lcd->_entrymode);
    
}


// -------------------------------------------------- //
// turn off auto shift

void LCDautoShiftOff(LCD * lcd) {
    
    lcd->_entrymode &= FLAG_ENTRY_NOAUTOSHIFT;
    LCDcommand(lcd, lcd->_entrymode);
    
}


// -------------------------------------------------- //
// turn display on

void LCDdisplayOn(LCD * lcd) {
    
    lcd->_displaycontrol |= FLAG_DISPLAYCONTROL_DISPLAYON;
    LCDcommand(lcd, lcd->_displaycontrol);
    
}

          
// -------------------------------------------------- //
// turn display off

void LCDdisplayOff(LCD * lcd) {
    
    lcd->_displaycontrol &= FLAG_DISPLAYCONTROL_DISPLAYOFF;
    LCDcommand(lcd, lcd->_displaycontrol);
    
}


// -------------------------------------------------- //
// turn cursor on

void LCDcursorOn(LCD * lcd) {
    
    lcd->_displaycontrol |= FLAG_DISPLAYCONTROL_CURSORON;
    LCDcommand(lcd, lcd->_displaycontrol);
    
}


// -------------------------------------------------- //
// turn cursor off

void LCDcursorOff(LCD * lcd) {
    
    lcd->_displaycontrol &= FLAG_DISPLAYCONTROL_CURSOROFF;
    LCDcommand(lcd, lcd->_displaycontrol);
    
}


// -------------------------------------------------- //
// turn cursor blink on

void LCDblinkOn(LCD * lcd) {
    
    lcd->_displaycontrol |= FLAG_DISPLAYCONTROL_BLINKON;
    LCDcommand(lcd, lcd->_displaycontrol);
    
}


// -------------------------------------------------- //
// turn cursor blink off

void LCDblinkOff(LCD * lcd) {
    
    lcd->_displaycontrol &= FLAG_DISPLAYCONTROL_BLINKOFF;
    LCDcommand(lcd, lcd->_displaycontrol);
    
}


// -------------------------------------------------- //
// shift cursor to the left, decrease address counter

void LCDshiftCursorLeft(LCD * lcd) {
    
    LCDcommand(lcd, (MASK_DISPLAYCURSORSHIFT & FLAG_DISPLAYCURSORSHIFT_SHIFTCURSOR) & FLAG_DISPLAYCURSORSHIFT_SHIFTLEFT);
    
}


// -------------------------------------------------- //
// shift cursor to the right, increase address counter

void LCDshiftCursorRight(LCD * lcd) {
    
    LCDcommand(lcd, (MASK_DISPLAYCURSORSHIFT & FLAG_DISPLAYCURSORSHIFT_SHIFTCURSOR) | FLAG_DISPLAYCURSORSHIFT_SHIFTRIGHT);
    
}


// -------------------------------------------------- //
// shift entire display to the left, cursor moves
// according to the display

void LCDshiftDisplayLeft(LCD * lcd) {
    
    LCDcommand(lcd, (MASK_DISPLAYCURSORSHIFT | FLAG_DISPLAYCURSORSHIFT_SHIFTDISPLAY) & FLAG_DISPLAYCURSORSHIFT_SHIFTLEFT);
    
}


// -------------------------------------------------- //
// shift entire display to the right, cursor moves
// according to the display

void LCDshiftDisplayRight(LCD * lcd) {
    
    LCDcommand(lcd, (MASK_DISPLAYCURSORSHIFT | FLAG_DISPLAYCURSORSHIFT_SHIFTDISPLAY) | FLAG_DISPLAYCURSORSHIFT_SHIFTRIGHT);
    
}


// -------------------------------------------------- //
// create a custom character

void LCDcustomCharacter(); // TBD


// -------------------------------------------------- //
// set the cursor to a position by setting the address 
// counter to the corresponding DDRAM address

void LCDsetCursorPosition(LCD * lcd, uint8_t target_row, uint8_t target_col) {
    
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
    
    LCDcommand(lcd, MASK_SETDDRAMADDR | (lcd->_row_offset[target_row] + target_col));
    
}


// -------------------------------------------------- //
// sends a command to the LCD

void LCDcommand(LCD * lcd, uint8_t command) {
    
    LCDsend(lcd, command, 0);
    
}


// -------------------------------------------------- //
// sends a character to the LCD

void LCDcharacter(LCD * lcd, uint8_t data) {
    
    LCDsend(lcd, data, 1);
    
}


// -------------------------------------------------- //
// sends a string to the LCD

void LCDprint(LCD * lcd, char * data) {
    
    for (int i = 0; data[i] != '\0'; i++) {
        
        LCDcharacter(lcd, data[i]);
        
    }
    
}


// -------------------------------------------------- //
// sends a 1 byte message of type (command/data) to 
// the LCD and automatically picks the right mode (4-
// or 8-bit)

void LCDsend(LCD * lcd, uint8_t message, uint8_t type) {
    
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
    
    // pull the rw pin low if applicable
    if (lcd->_rw_pin != 0xFF) {
    
        clear_io_bit(PORTB, lcd->_rw_pin);
    
    }
    
    // send the data depending on bus width
    switch (lcd->_displayfunction & FLAG_FUNCTIONSET_8BITBUS) {
        
        // 4 bit bus
        case 0:
            
            // send 4 msb first
            LCDsend4bit(lcd, (message >> 4));
            // send 4 lsb last
            LCDsend4bit(lcd, message);
            break;
        
        // 8 bit bus
        default:
            
            LCDsend8bit(lcd, message);
            break;
            
    }
    
}


// -------------------------------------------------- //
// sends an 8-bit message to the LCD

void LCDsend8bit(LCD * lcd, uint8_t message) {
    
    // extract i'th bit of the message by shifting it to
    // the right i times and then & with 1 (0b00000001)
    for (int i = 0; i < 8; i++) {
                
                change_io_bit(PORTD, lcd->_data_bus[i], ((message >> i) & 1));
                
    }

    LCDbeginTransfer(lcd);
    
}


// -------------------------------------------------- //
// sends a 4-bit message to the LCD

void LCDsend4bit(LCD * lcd, uint8_t message) {
    
    // see LCDsend8bit for explanation
    for (int i = 0; i < 4; i++) {
                
                change_io_bit(PORTD, lcd->_data_bus[i], ((message >> i) & 1));
                
    }

    LCDbeginTransfer(lcd);
    
}


// -------------------------------------------------- //
// sends a pulse to the enable pin

void LCDbeginTransfer(LCD * lcd) {
    
    // 1. pull the pin low and wait for the LCD
    clear_io_bit(PORTB, lcd->_en_pin);
    _delay_us(1);
    
    // 2. pull it high, the enable pulse needs to be >450ns
    set_io_bit(PORTB, lcd->_en_pin);
    _delay_us(1);
    
    // 3. pull it low again and wait for the LCD (>37 us)
    clear_io_bit(PORTB, lcd->_en_pin);
    _delay_us(100);
    
}