// ------------------------------------------------------------ //
// dependencies

# include <avr/io.h>
# include <avr/interrupt.h>
# include <util/delay.h>

# include "lcd.h"


// ------------------------------------------------------------ //
// constants

# ifndef F_CPU
# define F_CPU 16000000UL
# endif


// ------------------------------------------------------------ //
// main

int main (void) {
    
    LCD testlcd;
    
    DDRD = (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5);
    DDRB = (1 << PB0) | (1 << PB1) | (1 << PB2);
    
    LCDconfig(&testlcd, PB0, PB1, PB2, PD2, PD3, PD4, PD5, 0, 0, 0, 0);
    LCDinit(&testlcd, 4, 2, 16);
    
    return 0;
    
}