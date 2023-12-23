CPUFREQ      = 16000000
BAUD         = 115200
MCU          = atmega328p
PROGRAMMER   = arduino
PORT         = /dev/serial/by-path/pci-0000\:00\:14.0-usb-0\:1\:1.0
CFLAGS       = -c -std=gnu99 -Os -Wall -ffunction-sections -fdata-sections -mmcu=$(MCU) -DF_CPU=$(CPUFREQ)
LFLAGS       = -Os -mmcu=$(MCU) -Wl,--gc-sections
OBJCOPYFLAGS = -O ihex -R .eeprom
AVRDUDEFLAGS = -v -p $(MCU) -c $(PROGRAMMER) -b $(BAUD) -P $(PORT) -U flash:w:$(MAINFILE).ihex
MAINFILE     = main


default: main.o link converttohex upload clean


main.o: $(MAINFILE).c
	
	avr-gcc $(CFLAGS) $(MAINFILE).c -o $(MAINFILE).o


link: $(MAINFILE).o
	
	avr-gcc $(LFLAGS) $(MAINFILE).o -o $(MAINFILE).elf


converttohex: $(MAINFILE).elf
	
	avr-objcopy $(OBJCOPYFLAGS) $(MAINFILE).elf $(MAINFILE).ihex


upload: $(MAINFILE).ihex
	
	avrdude $(AVRDUDEFLAGS)
	
	
clean:
	
	rm $(MAINFILE).o $(MAINFILE).elf $(MAINFILE).ihex