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
ELF = $(PROGRAM).elf
BINARY = $(PROGRAM).bin

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

##############################################################
#The driver sources
##############################################################
SOURCE += $(DRIVERSDIR)/gpio.c
SOURCE += $(DRIVERSDIR)/led.c
SOURCE += $(DRIVERSDIR)/button.c
SOURCE += $(DRIVERSDIR)/usart.c
SOURCE += $(DRIVERSDIR)/delay.c
SOURCE += $(DRIVERSDIR)/pm.c
SOURCE += $(DRIVERSDIR)/spi.c
SOURCE += $(DRIVERSDIR)/dip204.c
SOURCE += $(DRIVERSDIR)/at45db642.c

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
	$(KERNELDIR)/rosa_int_asm.S \
	$(STARTUPDIR)/startup/crt0.S \
	$(KERNELDIR)/rosa_tim_asm.S \
	$(KERNELDIR)/rosa_ker_asm.S \
	$(DRIVERSDIR)/pot.S \

##############################################################
#The system sources of ROSA
##############################################################
SYSTEMSOURCE= \
	$(SYSTEMDIR)/semaphore.c \
	$(SYSTEMDIR)/heap.c


##############################################################
#Application sources that uses ROSA
##############################################################
APPSOURCE= \
#	$(APPDIR)/warning.c \ todo:

##############################################################
#Header files are located in these files
##############################################################
INCDIRS = -Isrc -Isrc/include

##############################################################
#Binaries are located in this dir
##############################################################
BINDIR = bin
ELFDIR = .

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
SIZE = avr32-size

##############################################################
#JTAG tool which will be used
##############################################################
JTAGTOOL = avrdragon
PROGRAMMER = avr32program

##############################################################
#Various compile flags etc
##############################################################
LDSCRIPT = $(STARTUPDIR)/linkscript/uc3a0512.lds
DEBUG = -ggdb
OPT = 3
AFLAGS = -x assembler-with-cpp
#~ CFLAGS = $(DEBUG) -DO$(OPT) -Wall -Wa,-R -mrelax -c -muse-rodata-section -msoft-float -mpart=$(PART) -DBOARD=$(BOARD) -fdata-sections -ffunction-sections $(INCDIRS) -nostartfiles
#~ LDFLAGS = --gc-sections --relax --direct-data -nostartfiles -mpart=$(PART) -T$(LDSCRIPT)

CFLAGS = -c
CFLAGS += -mpart=$(PART) -DBOARD=$(BOARD)
CFLAGS += $(INCDIRS)
CFLAGS += $(DEBUG) -DO$(OPT)
CFLAGS += -Wall -Wa,-R
CFLAGS += -msoft-float
CFLAGS += -nostartfiles
CFLAGS += -fomit-frame-pointer
CFLAGS += -mrelax
CFLAGS += -fno-common
CFLAGS += -fsection-anchors
#~ CFLAGS += -fdata-sections
CFLAGS += -ffunction-sections
CFLAGS += -mno-use-rodata-section
#~ CFLAGS += -Wa,--linkrelax
#~ CFLAGS += -Wa,--pic
#~ CFLAGS += -Wl,--pic

LDFLAGS = --gc-sections --relax --direct-data -nostartfiles -mpart=$(PART) -T$(LDSCRIPT)

OBJ = $(ASMSOURCE:%.S=%.o) $(SOURCE:%.c=%.o) $(SYSTEMSOURCE:%.c=%.o) $(APPSOURCE:%.c=%.o)

TEXTPRECOMPILE = @echo -n "---  $< "
TEXTPOSTCOMPILE = @echo -e "\r[OK]"

BUILDDIR = .

##############################################################
#Makefile rules
##############################################################
.PHONY: all
all: $(OBJ) $(ELFDIR)/$(ELF) $(BINARY) ok size crlf

print:
	@echo $(OBJ)

ok:
	@echo -e "\r[ALL OK]"

crlf:
	@echo

#Never optimiz the delay_ms function in src/drivers/delay.c
$(DRIVERSDIR)/delay.o: $(DRIVERSDIR)/delay.c
	$(TEXTPRECOMPILE) with -O0
	@$(TEST) -d $(BUILDDIR)/$(@D) || $(MKDIR) $(BUILDDIR)/$(@D)
	@$(CC) $(CFLAGS) -O0 $< -o$(BUILDDIR)/$@
	$(TEXTPOSTCOMPILE)

%.o: %.S
	$(TEXTPRECOMPILE)
	@$(TEST) -d $(BUILDDIR)/$(@D) || $(MKDIR) -p $(BUILDDIR)/$(@D)
	@$(CC) $(CFLAGS) $(AFLAGS) -O$(OPT) $< -o$(BUILDDIR)/$@
	$(TEXTPOSTCOMPILE)

#~ %.c: $(DEPS)
#~ @echo AHH, SOMEONE THOUGHT THEY WERE CLEVER
#~ %.o

%.o: %.c
	$(TEXTPRECOMPILE)
	@$(TEST) -d $(BUILDDIR)/$(@D) || $(MKDIR) -p $(BUILDDIR)/$(@D)
	@$(CC) $(CFLAGS) -O$(OPT) $< -o$(BUILDDIR)/$@
	$(TEXTPOSTCOMPILE)

$(BINARY): ./rosa.elf
	$(OBJCOPY) -O binary $(ELFDIR)/$(ELF) $(BINDIR)/$(BINARY)

./rosa.elf: $(OBJ)
	@$(TEST) -d $(ELFDIR) || $(MKDIR) -p $(ELFDIR)
	@echo -n "...  Linking $(ELF)"
	@$(CC) $(LDFLAGS) $(OBJ:%=$(BUILDDIR)/%) -o $(ELFDIR)/$(ELF)
	$(TEXTPOSTCOMPILE)

.PHONY: program
program: $(BINDIR)/$(BINARY)
	#erase sectors [-e], program internal flash [-f] at offset [-O] 0x80000000 using the xtal as clock [-cxtal], verify [-v], reset [-R] and run [-r]
	$(PROGRAMMER) program -O0x80000000 -finternal@0x80000000 -e -v -cxtal -Rr $(BINDIR)/$(BINARY)

.PHONY: reset
reset:
	$(PROGRAMMER) reset

.PHONY: run
run:
	$(PROGRAMMER) run

.PHONY: cpuinfo
cpuinfo:
	$(PROGRAMMER) cpuinfo

.PHONY: size
size: $(BINARY)
	@$(SIZE) --target=binary $(BINDIR)/$(BINARY)

.PHONY: gdb
gdb:
	avr32gdbproxy -k -a localhost:4711 -cUSB -e$(JTAGTOOL)

.PHONY: kill
kill:
	killall avr32gdbproxy

.PHONY: killall
	killall avr32gdbproxy

.PHONY: dump
dump:
	$(OBJDUMP) -S -x $(ELFDIR)/$(ELF)|$(LESS)

.PHONY: clean
clean:
	rm -f $(OBJ) $(ELFDIR)/$(ELF) $(BINDIR)/$(BINARY)

.PHONY: semiclean
semiclean:
	rm -f bin/rosa.bin rosa.elf src/drivers/spi.o src/drivers/at45db642.o

.PHONY: test
test: src/system/list.c src/include/system/list.h src/test/listtest.c
	gcc -ggdb -Isrc/include src/system/list.c src/test/listtest.c -otest


