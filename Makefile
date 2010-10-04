#*****************************************************************************
#
#                 ,//////,   ,////    ,///' /////,
#                ///' ./// ///'///  ///,    ,, //
#               ///////,  ///,///   '/// ///''//,
#             ,///' '///,'/////',/////'  /////'//,
#
#    Copyright 2010 Marcus Jansson <mjansson256@yahoo.se>
#
#    This file is part of ROSA - Realtime Operating System for AVR32.
#
#    ROSA is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    ROSA is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with ROSA.  If not, see <http://www.gnu.org/licenses/>.
#*****************************************************************************

PROGRAM = rosa
BOARD = EVK1100
PART = uc3a0512

SOURCEDIR= src
BINDIR = bin
STARTUPDIR = $(SOURCEDIR)/cpu
KERNELDIR = $(SOURCEDIR)/kernel
DRIVERSDIR = $(SOURCEDIR)/drivers
INCDIRS = -Isrc -Isrc/include

CC = avr32-gcc
LD = avr32-ld
AS = avr32-as
OBJCOPY = avr32-objcopy

DEBUG = -ggdb
OPT = -O0
AFLAGS = -x assembler-with-cpp
CFLAGS = $(DEBUG) $(OPT) -Wall -c -muse-rodata-section -msoft-float -mpart=$(PART) -DBOARD=$(BOARD) -fdata-sections -ffunction-sections $(INCDIRS) -nostartfiles
LDFLAGS = --gc-sections --direct-data -nostartfiles -mpart=$(PART) -T$(LDSCRIPT)
LDSCRIPT = $(STARTUPDIR)/linkscript/link_uc3a0512.lds

OBJ=$(STARTUPDIR)/startup/crt0.o \
	$(KERNELDIR)/rosa_int.o \
	$(KERNELDIR)/rosa_tim.o \
	$(KERNELDIR)/rosa_ker.o \
	$(KERNELDIR)/rosa_asm.o \
	$(DRIVERSDIR)/pot.o \
	$(DRIVERSDIR)/gpio.o \
	$(DRIVERSDIR)/led.o \
	$(DRIVERSDIR)/button.o \
	$(DRIVERSDIR)/usart.o \
	$(SOURCEDIR)/main.o

all: clean $(OBJ) elf $(PROGRAM)

$(STARTUPDIR)/startup/crt0.o: $(STARTUPDIR)/startup/crt0.S
	$(CC) $(CFLAGS) $(AFLAGS)  $< -o$@

$(KERNELDIR)/rosa_int.o: $(KERNELDIR)/rosa_int.S
	$(CC) $(CFLAGS) $(AFLAGS) $< -o$@

$(KERNELDIR)/rosa_asm.o: $(KERNELDIR)/rosa_asm.S
	$(CC) $(CFLAGS) $(AFLAGS) $< -o$@

$(DRIVERSDIR)/pot.o: $(DRIVERSDIR)/pot.S
	$(CC) $(CFLAGS) $(AFLAGS) $< -o$@

$(KERNELDIR)/rosa_tim.o: $(KERNELDIR)/rosa_tim.S
	$(CC) $(CFLAGS) $(AFLAGS) $< -o$@

$(PROGRAM):
	$(OBJCOPY) -O binary $(BINDIR)/$(PROGRAM).elf $(BINDIR)/$(PROGRAM).bin

elf:
	$(CC) $(LDFLAGS)  $(OBJ) -o $(BINDIR)/$(PROGRAM).elf

program: $(BINDIR)/$(PROGRAM).bin
	@avr32program program -O0x80000000 -finternal@0x80000000 -e -v -cxtal -Rr $(BINDIR)/$(PROGRAM).bin

gdb:
	avr32gdbproxy -t localhost:4712 -a localhost:4711 -cUSB -eavrdragon

clean:
	rm -f $(OBJ) $(BINDIR)/$(PROGRAM).elf $(BINDIR)/$(PROGRAM).bin
