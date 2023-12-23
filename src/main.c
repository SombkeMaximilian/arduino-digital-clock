// ------------------------------------------------------------ //
// dependencies

# include <avr/io.h>
# include <avr/interrupt.h>
# include <util/delay.h>


// ------------------------------------------------------------ //
// constants

# ifndef F_CPU
# define F_CPU 16000000UL
# endif
# define mskDDRD (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5)


// ------------------------------------------------------------ //
// main

int main (void) {
    
    setup();
    
    main_loop();
    
    return 0;
    
}


// ------------------------------------------------------------ //
// main loop of the program

void main_loop (void) {
    
    while (1) {
        
        PORTD ^= mskDDRD;
        _delay_ms(1000);
        
    }
    
}


// ------------------------------------------------------------ //
// configures the arduino pins

void setup (void) {
    
    DDRD = mskDDRD;
    
}