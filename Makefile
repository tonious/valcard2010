# Name: Makefile
# Project: EasyLogger
# Author: Christian Starkjohann
# Creation Date: 2007-06-23
# Tabsize: 4
# Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GPLv2.
# This Revision: $Id: Makefile 362 2007-06-25 14:38:21Z cs $

DEVICE=atmega48
AVRDUDE = avrdude -c usbtiny -p $(DEVICE) -B 1
# The two lines above are for "avrdude" and the STK500 programmer connected
# to an USB to serial converter to a Mac running Mac OS X.
# Choose your favorite programmer and interface.

CC = avr-gcc
CFLAGS = -std=c99 -Wall -Os -I. -Iusbdrv -mmcu=$(DEVICE) -DF_CPU=8000000 -DDEBUG_LEVEL=0
LDFLAGS = -Os -mmcu=$(DEVICE)
# NEVER compile the final product with debugging! Any debug output will
# distort timing so that the specs can't be met.


# symbolic targets:
all:	blink.hex eh.hex alphabet.hex stdiotest.hex buttontest.hex sleeptest.hex valcard.hex

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:	%.S
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

%.o:	%.s
	$(CC) $(CFLAGS) -S $< -o $@

%.bin:	%.o 
	$(CC) $(LDFLAGS) -o $@ $^

%.hex:	%.bin
	avr-objcopy -j .text -j .data -O ihex $< $@
	./checksize $< 4096 512
# do the checksize script as our last action to allow successful compilation
# on Windows with WinAVR where the Unix commands will fail.

flash_%: %.hex
	$(AVRDUDE) -U flash:w:$<:i

# FUSES ARE DEFAULT HIGH, SET LOW
fuse:
	$(AVRDUDE) -U hfuse:w:0xde:m -U lfuse:w:0xe2:m -U efuse:w:0x01:m

erase: 
	$(AVRDUDE) -e

clean:
	rm -f *.bin *.hex *.o

# file targets:

ledmatrix.o: font.h

stdiotest.bin: ledmatrix.o

buttontest.bin: ledmatrix.o

sleeptest.bin: ledmatrix.o

valcard.bin: ledmatrix.o
