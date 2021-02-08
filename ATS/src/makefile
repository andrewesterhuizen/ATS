FILENAME   = ATS
PORT       = usb
DEVICE     = atmega328p
PROGRAMMER = usbasp
BAUD       = 115200
COMPILE    = avr-gcc -Wall -Os -mmcu=$(DEVICE)

# fuses = (E:FF, H:D9, L:F7)

default: compile upload clean

compile:
	$(COMPILE) -c $(FILENAME).cpp -o $(FILENAME).o -std=c++11
	$(COMPILE) -o $(FILENAME).elf $(FILENAME).o 
	avr-objcopy -j .text -j .data -O ihex $(FILENAME).elf $(FILENAME).hex 
	avr-size --format=avr --mcu=$(DEVICE) $(FILENAME).elf

upload:
	avrdude -v -p $(DEVICE) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -s -U flash:w:$(FILENAME).hex

clean:
	rm $(FILENAME).o
	rm $(FILENAME).elf
	rm $(FILENAME).hex