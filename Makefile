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

ASMSOURCE= \
	$(STARTUPDIR)/startup/crt0.S \
	$(KERNELDIR)/rosa_int_asm.S \
	$(KERNELDIR)/rosa_tim_asm.S \
	$(KERNELDIR)/rosa_ker_asm.S \
	$(DRIVERSDIR)/pot.S \

SOURCE= \
	$(KERNELDIR)/rosa_int.c \
	$(KERNELDIR)/rosa_tim.c \
	$(KERNELDIR)/rosa_ker.c \
	$(KERNELDIR)/rosa_scheduler.c \
	$(DRIVERSDIR)/gpio.c \
	$(DRIVERSDIR)/led.c \
	$(DRIVERSDIR)/button.c \
	$(DRIVERSDIR)/usart.c \
	$(DRIVERSDIR)/delay.c \
	$(SOURCEDIR)/main.c

OBJ = $(ASMSOURCE:%.S=%.o) $(SOURCE:%.c=%.o)

all: print clean $(OBJ) elf $(PROGRAM)

print:
	@echo $(OBJ)
	@echo

%.o: %.S
	$(CC) $(CFLAGS) $(AFLAGS)  $< -o$@
	@echo

%.o: %.c
	$(CC) $(CFLAGS) $< -o$@
	@echo

$(PROGRAM):
	$(OBJCOPY) -O binary $(BINDIR)/$(PROGRAM).elf $(BINDIR)/$(PROGRAM).bin

elf:
	@test -d $(BINDIR) || mkdir $(BINDIR)
	$(CC) $(LDFLAGS)  $(OBJ) -o $(BINDIR)/$(PROGRAM).elf

dump:
	avr32-objdump -S -x bin/rosa.elf |less

program: $(BINDIR)/$(PROGRAM).bin
	@avr32program program -O0x80000000 -finternal@0x80000000 -e -v -cxtal -Rr $(BINDIR)/$(PROGRAM).bin

run: 
	avr32program run
	
reset:
	avr32program reset

gdb:
	avr32gdbproxy -t localhost:4712 -a localhost:4711 -cUSB -eavrdragon

kill:
	killall avr32gdbproxy

clean:
	rm -f $(OBJ) $(BINDIR)/$(PROGRAM).elf $(BINDIR)/$(PROGRAM).bin
	@echo
