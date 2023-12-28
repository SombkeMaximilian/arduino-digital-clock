CPUFREQ      = 16000000
BAUD         = 115200

MCU          = atmega328p
PROGRAMMER   = arduino

PORT         = /dev/serial/by-path/pci-0000\:00\:14.0-usb-0\:1\:1.0

CFLAGS       = -c -std=gnu99 -Os -Wall -ffunction-sections -fdata-sections -mmcu=$(MCU) -DF_CPU=$(CPUFREQ)
LFLAGS       = -Os -mmcu=$(MCU) -Wl,--gc-sections
OBJCOPYFLAGS = -O ihex -R .eeprom
AVRDUDEFLAGS = -C /etc/avrdude.conf -v -p $(MCU) -c $(PROGRAMMER) -b $(BAUD) -P $(PORT)

MAINFILENAME = main
LCDFILENAME  = lcd
RTCFILENAME  = ds1302


default: compile link converttohex upload clean


compile: $(MAINFILENAME).c $(LCDFILENAME).c $(LCDFILENAME).h $(RTCFILENAME).c $(RTCFILENAME).o

	avr-gcc $(CFLAGS) $(MAINFILENAME).c -o $(MAINFILENAME).o
	avr-gcc $(CFLAGS) $(LCDFILENAME).c -o $(LCDFILENAME).o
	avr-gcc $(CFLAGS) $(RTCFILENAME).c -o $(RTCFILENAME).o


link: $(MAINFILENAME).o $(LCDFILENAME).o $(RTCFILENAME).o
	
	avr-gcc $(LFLAGS) $(MAINFILENAME).o $(LCDFILENAME).o $(RTCFILENAME).o -o $(MAINFILENAME).elf


converttohex: $(MAINFILENAME).elf
	
	avr-objcopy $(OBJCOPYFLAGS) $(MAINFILENAME).elf $(MAINFILENAME).ihex


upload: $(MAINFILENAME).ihex
	
	avrdude $(AVRDUDEFLAGS) -U flash:w:$(MAINFILENAME).ihex
	
	
clean:
	
	rm *.o *.elf *.ihex