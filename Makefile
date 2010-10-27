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
# Tab size: 4

##############################################################
# The name of the program
##############################################################
PROGRAM = rosa

##############################################################
#Sources are located in this dir
##############################################################
SOURCEDIR = src

##############################################################
#Various source directories
##############################################################
STARTUPDIR = $(SOURCEDIR)/cpu
KERNELDIR = $(SOURCEDIR)/kernel
DRIVERSDIR = $(SOURCEDIR)/drivers

##############################################################
#The sources of ROSA
##############################################################
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

##############################################################
#The assembler sources of ROSA
##############################################################
ASMSOURCE= \
	$(STARTUPDIR)/startup/crt0.S \
	$(KERNELDIR)/rosa_int_asm.S \
	$(KERNELDIR)/rosa_tim_asm.S \
	$(KERNELDIR)/rosa_ker_asm.S \
	$(DRIVERSDIR)/pot.S \

##############################################################
#Header files are located in these files
##############################################################
INCDIRS = -Isrc -Isrc/include

##############################################################
#binaries are located in this dir
##############################################################
BINDIR = bin

##############################################################
# The target board and MCU
##############################################################
BOARD = EVK1100
PART = uc3a0512

##############################################################
#Various build programs
##############################################################
CC = avr32-gcc
LD = avr32-ld
AS = avr32-as
OBJCOPY = avr32-objcopy
TEST = test
MKDIR = mkdir

##############################################################
#Various compile flags etc
##############################################################
DEBUG = -ggdb
OPT = -O0
AFLAGS = -x assembler-with-cpp
CFLAGS = $(DEBUG) $(OPT) -Wall -c -muse-rodata-section -msoft-float -mpart=$(PART) -DBOARD=$(BOARD) -fdata-sections -ffunction-sections $(INCDIRS) -nostartfiles
LDFLAGS = --gc-sections --direct-data -nostartfiles -mpart=$(PART) -T$(LDSCRIPT)
LDSCRIPT = $(STARTUPDIR)/linkscript/link_uc3a0512.lds
OBJ = $(ASMSOURCE:%.S=%.o) $(SOURCE:%.c=%.o)

##############################################################
#Makefile rules
##############################################################
all: $(OBJ) elf $(PROGRAM)

%.o: %.S
	$(CC) $(CFLAGS) $(AFLAGS)  $< -o$@

%.o: %.c
	$(CC) $(CFLAGS) $< -o$@

$(PROGRAM):
	$(OBJCOPY) -O binary $(BINDIR)/$(PROGRAM).elf $(BINDIR)/$(PROGRAM).bin

elf:
	@$(TEST) -d $(BINDIR) || $(MKDIR) $(BINDIR)
	$(CC) $(LDFLAGS)  $(OBJ) -o $(BINDIR)/$(PROGRAM).elf

program: $(BINDIR)/$(PROGRAM).bin
	avr32program program -O0x80000000 -finternal@0x80000000 -e -v -cxtal -Rr $(BINDIR)/$(PROGRAM).bin

gdb:
	avr32gdbproxy -t localhost:4712 -a localhost:4711 -cUSB -eavrdragon

clean:
	rm -f $(OBJ) $(BINDIR)/$(PROGRAM).elf $(BINDIR)/$(PROGRAM).bin

