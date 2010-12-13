/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// //;''//,
             ,///' '///,'/////',/////'  /////'/;,

    Copyright 2010 Marcus Jansson <mjansson256@yahoo.se>

    This file is part of ROSA - Realtime Operating System for AVR32.

    ROSA is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ROSA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ROSA.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "kernel/rosa_dyn.h"
#include "drivers/lcd.h"
#include "drivers/led.h"
#include "drivers/spi.h"
#include "drivers/delay.h"	//TODO: consider something irq-ish for delay
//~ #include "compiler.h"	//TODO: REMOVE! >:l
#include "drivers/pot.h"

void dip204_wait_busy(void);
void dip204_write_byte(int byte);
void dip204_read_byte(unsigned int * byte);
//~ int bit_reverse8(int word);

/*************************************************************
 * task_dip204_backlight
 * Comment:
 * Crappy Soft-PWM for the LCD backlight.
 * Just for testing purposes.
 *************************************************************/
void task_dip204_backlight(void * param)
{
	int led = (int)param;
	while(1) {
		ledToggle(led);
		ROSA_wait(potGetValue());
	}
}



/**
 * dip204 controller Samsung KS0073
 * Ratings:
 * 						min		max		unit
 * SCK					1		20		us
 * SCK rise fall time			50		ns
 * SCK width			400				ns
 * CS setup time		60				ns
 * CS hold time			20				ns
 * Serial input time	200				ns
 * Serial input hold	200				ns
 * Serial output delay			360		ns
 * Serial output hold	5				ns
 */
/**
Table 2. Various kinds of operations according to RS and R/W bits.
     RS           R/W      Operation
      0            0       Instruction Write operation (MPU writes Instruction code into IR)
      0            1       Read Busy flag (DB7) and address counter (DB0 ~ DB6)
      1            0       Data Write operation (MPU writes data into DR)
      1            1       Data Read operation (MPU reads data from DR)
*/

#define DIP204_SPI (&AVR32_SPI1)
#define DIP204_CS 2
#define DIP204_CS_PIN AVR32_SPI1_NPCS_2_0_PIN
#define DIP204_CS_FUNCTION AVR32_SPI1_NPCS_2_0_FUNCTION
#define DIP204_BACKLIGHT AVR32_PIN_PB18

typedef enum {
	DIP204_WRITE_COMMAND = 0xf8,
	DIP204_WRITE_DATA = 0xfa,
	DIP204_READ_COMMAND = 0xfc,
	DIP204_READ_DATA = 0xfe,
} dip204_commands_t;


void dip204_write_byte(int byte)
{
	switch(byte) {
		//MSB commands
		case DIP204_READ_COMMAND:
		case DIP204_READ_DATA:
		case DIP204_WRITE_COMMAND:
		case DIP204_WRITE_DATA:
			//Send D7-d0
			spiWriteByte(DIP204_SPI, byte);
			break;
		default:
			//LSB commands
			asm("brev\t%0" : "+r" (byte) :  : "cc");
			byte >>= 0x18;
			//~ byte = bit_reverse8(byte) >> 0x18;	//Reverse and put the byte where it should be
			spiWriteByte(DIP204_SPI, byte & 0xf0);
			spiWriteByte(DIP204_SPI, (byte << 4) & 0xf0);
			break;
		}
}

void dip204_read_byte(unsigned int * byte)
{
	//~ dip204_select();
	spiWriteByte(DIP204_SPI, 0x00);
	spiReadByte(DIP204_SPI, byte);
	asm("brev\t%0" : "+r" (*byte) :  : "cc");
	*byte >>= 0x18;
	//~ *byte = bit_reverse8(*byte) >> 0x18;
}

void dip204_wait_busy(void)
{
	unsigned int status = 0x00;

	/* send read commd to LCD */
	dip204_write_byte(DIP204_READ_COMMAND);
	/* read next byte */
	do {
		dip204_read_byte(&status);
		/* look at D7 to know status */
	} while (status & 0x80);
}

void dip204_backlight(int onOff)
{
	switch(onOff) {
		case ON:
			gpioClear(DIP204_BACKLIGHT);
			break;
		case OFF:
			gpioSet(DIP204_BACKLIGHT);
			break;
		default:
			gpioToggle(DIP204_BACKLIGHT);
			break;
		}
}

void dip204_select()
{
	spiChipSelect(DIP204_SPI, DIP204_CS);
}

void dip204_unselect()
{
	spiChipDeselect(DIP204_SPI);
}

void dip204_set_cursor_position(unsigned char column, unsigned char line)
{
  unsigned char address = 0;

  dip204_select();
  if ((column <= 20) && (line <= 4))
  {
    /* Calculate DDRAM address from line and row values */
    address = ( (line-1) * 32 ) + ( column-1 ) + 128;
  }
  /* Send Command Start Byte */
  dip204_write_byte(DIP204_WRITE_COMMAND);
  /* Send Adress lower Nibble */
  dip204_write_byte(address);
  dip204_wait_busy();
  dip204_unselect();
}

void dip204Init(void)
{
	// delay for power on
	delay_ms(100);

	//SPI init
	spiEnable(DIP204_SPI, DIP204_CS, DIP204_CS_PIN, DIP204_CS_FUNCTION);

	// select the LCD chip
	spiChipSelect(DIP204_SPI, DIP204_CS);

	// Send Command Start Byte
	dip204_write_byte(DIP204_WRITE_COMMAND);
	// Send "extended Function Set" Command  (RE=1)
	dip204_write_byte(0x34);
	// Wait for command execution
	delay_ms(1);
	// Send "Enter 4-Line Mode" Command
	dip204_write_byte(0x09);
	// Wait for command execution
	delay_ms(1);
	// Send "Function Set" Command (RE=0)
	dip204_write_byte(0x30);
	// Wait for command execution
	delay_ms(1);
	// Send "Display On Command: Display On, Cursor On, Blink On"
	dip204_write_byte(0x0F);
	// Wait for command execution
	delay_ms(1);
	// Send "Display Clear" Command
	dip204_write_byte(0x01);
	// Wait for command execution
	delay_ms(5);
	// Send "Entry Mode Set Command: Increment Mode, Entire Shift off"
	dip204_write_byte(0x06);
	// Wait for command execution
	delay_ms(1);
	dip204_wait_busy();

	// unselect chip
	spiChipDeselect(DIP204_SPI);

	//LCD backlight on
	int prio = 1;
	int stacksize = 0x20;
	gpioInit(DIP204_BACKLIGHT, OUTPUT);
	ROSA_taskCreate("lcdl", task_dip204_backlight, (void *)DIP204_BACKLIGHT, prio, stacksize);
	//~ dip204_backlight(ON);
}

void dip204_write_string(const char * string)
{
	unsigned char i=0;

	dip204_select();
	/* for all chars in string */
	while(string[i]!= '\0')
	{
		/* Send Write Data Start Byte */
		dip204_write_byte(DIP204_WRITE_DATA);
		/* Send byte */
		dip204_write_byte(string[i]);
		/* go to next char */
		i++;
		dip204_wait_busy();
	}
	dip204_unselect();
}

void dip204_Welcome(void)
{
	dip204_set_cursor_position(1,2);
	dip204_write_string(" -=> emCode.se <=-");
	dip204_set_cursor_position(1,3);
	dip204_write_string("ROSA RTOS for AVR32");
}