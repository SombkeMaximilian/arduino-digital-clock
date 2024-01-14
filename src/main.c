// ------------------------------------------------------------ //
// dependencies

# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>
# include <string.h>
# include <avr/io.h>
# include <avr/interrupt.h>
# include <util/delay.h>

# include "lcd.h"
# include "ds1302.h"
# include "macros.h"


// ------------------------------------------------------------ //
// constants

# ifndef F_CPU
# define F_CPU 16000000UL
# endif


// ------------------------------------------------------------ //
// function that formats the time into a string

char * formatTime(timeData * data) {
    
    char * time = malloc(17 * sizeof(*time));
    
    sprintf(time, "%02d:%02d:%02d", bcd_to_dec(data->hour), bcd_to_dec(data->minute), bcd_to_dec(data->second));
    
    return time;
    
}


// ------------------------------------------------------------ //
// function that formats the date into a string

char * formatDate(timeData * data) {
    
    char * date = malloc(17 * sizeof(*date));
    char day[4];
    
    switch (data->dayofweek) {
        case 0:
            strcpy(day, "SUN");
            break;
        case 1:
            strcpy(day, "MON");
            break;
        case 2:
            strcpy(day, "TUE");
            break;
        case 3:
            strcpy(day, "WED");
            break;
        case 4:
            strcpy(day, "THU");
            break;
        case 5:
            strcpy(day, "FRI");
            break;
        case 6:
            strcpy(day, "SAT");
            break;
    }
    
    sprintf(date, "%s %02d.%02d.20%02d", day, bcd_to_dec(data->day), bcd_to_dec(data->month), bcd_to_dec(data->year));
    
    return date;
    
}


// ------------------------------------------------------------ //
// main

int main (void) {
    
    LCD lcd;
    DS1302 ds1302;
    timeData currDateTime;
    char * time;
    char * date;
    
    // set pins to output
    DDRD = (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6);
    DDRB = (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4);
    
    // configure and initialize the RTC
    DS1302config(&ds1302, PB3, PD6, PB4);
    DS1302init(&ds1302);
    
    // 24h mode
    DS1302setClockMode(&ds1302, 0);
    
    // get compile time and send it to the RTC module (maybe add an offset later)
    DS1302timeDataInit(&currDateTime, __DATE__, __TIME__);
    DS1302writeTimeData(&ds1302, &currDateTime);
    
    // start the clock
    DS1302start(&ds1302);
    
    // configure and initialize the LCD
    LCDconfig(&lcd, PB0, PB1, PB2, PD2, PD3, PD4, PD5, 0, 0, 0, 0);
    LCDinit(&lcd, 4, 2, 16);
    
    // loop that displays the clock
    while (1) {
        
        // read the data
        DS1302readTimeData(&ds1302, &currDateTime);
        
        // format it
        time = formatTime(&currDateTime);
        date = formatDate(&currDateTime);
        
        // return to position (0,0) and print the data
        LCDreturnHome(&lcd);
        LCDprint(&lcd, time);
        LCDsetCursorPosition(&lcd, 1, 0);
        LCDprint(&lcd, date);
        
        // free the memory
        free(time);
        free(date);
        
    }
    
    return 0;
    
}