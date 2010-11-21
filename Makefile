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
# The name of the program, elf and binary
##############################################################
PROGRAM = rosa
ELF = ../$(PROGRAM).elf
BINARY = t$(PROGRAM).bin

##############################################################
#Sources are located in this dir
##############################################################
SOURCEDIR = src

##############################################################
#Various source subdirectories
##############################################################
STARTUPDIR = $(SOURCEDIR)/cpu
KERNELDIR = $(SOURCEDIR)/kernel
SYSTEMDIR = $(SOURCEDIR)/system
DRIVERSDIR = $(SOURCEDIR)/drivers
APPDIR = $(SOURCEDIR)/app

##############################################################
#The kernel sources of ROSA (as in the student version)
##############################################################
SOURCE =  $(KERNELDIR)/rosa_int.c
SOURCE += $(KERNELDIR)/rosa_tim.c
SOURCE += $(KERNELDIR)/rosa_ker.c
SOURCE += $(KERNELDIR)/rosa_scheduler.c
SOURCE += $(DRIVERSDIR)/gpio.c
SOURCE += $(DRIVERSDIR)/led.c
SOURCE += $(DRIVERSDIR)/button.c
SOURCE += $(DRIVERSDIR)/usart.c
SOURCE += $(DRIVERSDIR)/delay.c

##############################################################
#Put your additional kernel source files here
##############################################################

SOURCE += $(KERNELDIR)/rosa_dyn.c
SOURCE += $(KERNELDIR)/rosa_systick.c

##############################################################
SOURCE += $(SOURCEDIR)/main.c

##############################################################
#The kernel assembler sources of ROSA
##############################################################
ASMSOURCE= \
	$(STARTUPDIR)/startup/crt0.S \
	$(KERNELDIR)/rosa_int_asm.S \
	$(KERNELDIR)/rosa_tim_asm.S \
	$(KERNELDIR)/rosa_ker_asm.S \
	$(DRIVERSDIR)/pot.S \

##############################################################
#The system sources of ROSA
##############################################################
SYSTEMSOURCE= \
	$(SYSTEMDIR)/semaphore.c \
	$(SYSTEMDIR)/heap.c
#	$(SYSTEMDIR)/list.c \
#	$(SYSTEMDIR)/queue.c \

##############################################################
#Application sources that uses ROSA
##############################################################
APPSOURCE= \
	$(APPDIR)/warning.c \

##############################################################
#Header files are located in these files
##############################################################
INCDIRS = -Isrc -Isrc/include

##############################################################
#Binaries are located in this dir
##############################################################
BINDIR = bin

##############################################################
#The target board and MCU
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
OBJDUMP = avr32-objdump
TEST = test
MKDIR = mkdir
LESS = less

##############################################################
#JTAG tool which will be used
##############################################################
JTAGTOOL = avrdragon
PROGRAMMER = avr32program

##############################################################
#Various compile flags etc
##############################################################
LDSCRIPT = $(STARTUPDIR)/linkscript/link_uc3a0512.lds
DEBUG = -ggdb
OPT = -O0
AFLAGS = -x assembler-with-cpp
CFLAGS = $(DEBUG) $(OPT) -Wall -c -muse-rodata-section -msoft-float -mpart=$(PART) -DBOARD=$(BOARD) -fdata-sections -ffunction-sections $(INCDIRS) -nostartfiles
LDFLAGS = --gc-sections --direct-data -nostartfiles -mpart=$(PART) -T$(LDSCRIPT)
OBJ = $(ASMSOURCE:%.S=%.o) $(SOURCE:%.c=%.o) $(SYSTEMSOURCE:%.c=%.o) $(APPSOURCE:%.c=%.o)

TEXTPRECOMPILE = @echo -n "---  $< "
TEXTPOSTCOMPILE = @echo -e "\r[OK]"

##############################################################
#Makefile rules
##############################################################
all: clean $(OBJ) elf $(BINARY)

print:
	@echo $(OBJ)

%.o: %.S
	$(TEXTPRECOMPILE)
	@$(CC) $(CFLAGS) $(AFLAGS)  $< -o$@
	$(TEXTPOSTCOMPILE)

%.o: %.c
	$(TEXTPRECOMPILE)
	@$(CC) $(CFLAGS) $< -o$@
	$(TEXTPOSTCOMPILE)

$(BINARY):
	$(OBJCOPY) -O binary $(BINDIR)/$(ELF) $(BINDIR)/$(BINARY)
	@echo -e "\r[ALL OK]"
	@echo

elf:
	@$(TEST) -d $(BINDIR) || $(MKDIR) $(BINDIR)
	@echo -n "...  Linking $(ELF)"
	@$(CC) $(LDFLAGS)  $(OBJ) -o $(BINDIR)/$(ELF)
	$(TEXTPOSTCOMPILE)

program: $(BINDIR)/$(BINARY)
	#erase sectors [-e], program internal flash [-f] at offset [-O] 0x80000000 using the xtal as clock [-cxtal], verify [-v], reset [-R] and run [-r]
	$(PROGRAMMER) program -O0x80000000 -finternal@0x80000000 -e -v -cxtal -Rr $(BINDIR)/$(BINARY)

reset:
	$(PROGRAMMER) reset

run:
	$(PROGRAMMER) run

cpuinfo:
	$(PROGRAMMER) cpuinfo

size:
	$(SIZE) --target=binary $(BINDIR)/$(BINARY)

gdb:
	avr32gdbproxy -k -a localhost:4711 -cUSB -e$(JTAGTOOL)

kill:
	killall avr32gdbproxy
dump:
	$(OBJDUMP) -S -x $(BINDIR)/$(ELF)|$(LESS)

clean:
	@rm -f $(OBJ) $(BINDIR)/$(ELF) $(BINDIR)/$(BINARY)

test: src/system/list.c src/include/system/list.h src/test/listtest.c
	gcc -ggdb -Isrc/include src/system/list.c src/test/listtest.c -otest


