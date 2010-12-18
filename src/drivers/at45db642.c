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
#include "drivers/at45db642.h"

#define AT45DB_SPI (&AVR32_SPI1)
enum {

	AT45DB_CS = 0,
	AT45DB_CS_FUNCTION = AVR32_SPI1_NPCS_0_0_FUNCTION,
	AT45DB_CS_PIN = AVR32_SPI1_NPCS_0_0_PIN,
	AT45DB_MOSI_PIN = AVR32_SPI1_MOSI_0_0_PIN,
	AT45DB_MOSI_FUNCTION = AVR32_SPI1_MOSI_0_0_FUNCTION,
	AT45DB_MISO_PIN = AVR32_SPI1_MISO_0_0_PIN,
	AT45DB_MISO_FUNCTION = AVR32_SPI1_MISO_0_0_FUNCTION,
	AT45DB_SPCK_PIN = AVR32_SPI1_SCK_0_0_PIN,
	AT45DB_SPCK_FUNCTION = AVR32_SPI1_SCK_0_0_FUNCTION
};

enum {
	AT45DB642_SR_PAGESIZE = (1 << 0),
	AT45DB642_SR_PROTECT  = (1 << 1),
	AT45DB642_SR_COMP     = (1 << 6),
	AT45DB642_SR_RDY      = (1 << 7)
};

/**
 * AT45DB commands
 */
enum {
	//Read commands
	AT45DB_MAIN_MEMORY_PAGE_READ_CMD = 0x2d,
	AT45DB_CONTINOUS_ARRAY_READ_LEGACY_CMD = 0xe8,
	AT45DB_CONTINOUS_ARRAY_READ_LOW_FREQ_CMD = 0x03,
	AT45DB_CONTINOUS_ARRAY_READ_CMD = 0x0b,
	AT45DB_BUFFER_1_READ_LOWFREQ_CMD = 0xd1,
	AT45DB_BUFFER_2_READ_LOWFREQ_CMD = 0xd3,
	AT45DB_BUFFER_1_READ_CMD = 0xd4,
	AT45DB_BUFFER_2_READ_CMD = 0xd6,

	//Program and erase commands
	AT45DB_BUFFER_1_WRITE_CMD = 0x84,
	AT45DB_BUFFER_2_WRITE_CMD = 0x87,
	AT45DB_BUFFER_1_TO_MAIN_MEMORY_PAGE_PROGRAM_WITH_BUILTIN_ERASE_CMD = 0x83,
	AT45DB_BUFFER_2_TO_MAIN_MEMORY_PAGE_PROGRAM_WITH_BUILTIN_ERASE_CMD = 0x86,
	AT45DB_BUFFER_1_TO_MAIN_MEMORY_PAGE_PROGRAM_CMD = 0x88,
	AT45DB_BUFFER_2_TO_MAIN_MEMORY_PAGE_PROGRAM_CMD = 0x89,
	AT45DB_PAGE_ERASE_CMD = 0x81,
	AT45DB_BLOCK_ERASE_CMD = 0x50,
	AT45DB_SECTOR_ERASE_CMD = 0x7c,
	AT45DB_MAIN_MEMORY_PAGE_PROGRAM_THROUGH_BUFFER_1_CMD = 0x82,
	AT45DB_MAIN_MEMORY_PAGE_PROGRAM_THROUGH_BUFFER_2_CMD = 0x85,

	//Protection and security commands
	AT45DB_ENABLE_SECTOR_PROTECTION_CMD0 = 0x3d,
	AT45DB_ENABLE_SECTOR_PROTECTION_CMD1 = 0x2a,
	AT45DB_ENABLE_SECTOR_PROTECTION_CMD2 = 0x7f,
	AT45DB_ENABLE_SECTOR_PROTECTION_CMD3 = 0xa9,
	AT45DB_DISABLE_SECTOR_PROTECTION_CMD0 = 0x3d,
	AT45DB_DISABLE_SECTOR_PROTECTION_CMD1 = 0x2a,
	AT45DB_DISABLE_SECTOR_PROTECTION_CMD2 = 0x7f,
	AT45DB_DISABLE_SECTOR_PROTECTION_CMD3 = 0x9a,
	AT45DB_ERASE_SECTOR_PROTECTION_REGISTER_CMD0 = 0x3d,
	AT45DB_ERASE_SECTOR_PROTECTION_REGISTER_CMD1 = 0x2a,
	AT45DB_ERASE_SECTOR_PROTECTION_REGISTER_CMD2 = 0x7f,
	AT45DB_ERASE_SECTOR_PROTECTION_REGISTER_CMD3 = 0xcf,
	AT45DB_PROGRAM_SECTOR_PROTECTION_REGISTER_CMD0 = 0x3d,
	AT45DB_PROGRAM_SECTOR_PROTECTION_REGISTER_CMD1 = 0x2a,
	AT45DB_PROGRAM_SECTOR_PROTECTION_REGISTER_CMD2 = 0x7f,
	AT45DB_PROGRAM_SECTOR_PROTECTION_REGISTER_CMD3 = 0xfc,
	AT45DB_READ_SECTOR_PROTECTION_REGISTER_CMD = 0x32,
	AT45DB_SECTOR_LOCKDOWN_CMD0 = 0x3d,
	AT45DB_SECTOR_LOCKDOWN_CMD1 = 0x2a,
	AT45DB_SECTOR_LOCKDOWN_CMD2 = 0x7f,
	AT45DB_SECTOR_LOCKDOWN_CMD3 = 0x30,
	AT45DB_READ_SECTOR_LOCKDOWN_REGISTER_CMD = 0x35,
	AT45DB_PROGRAM_SECURITY_REGISTER_CMD0 = 0x9b,
	AT45DB_PROGRAM_SECURITY_REGISTER_CMD1 = 0x00,
	AT45DB_PROGRAM_SECURITY_REGISTER_CMD2 = 0x00,
	AT45DB_PROGRAM_SECURITY_REGISTER_CMD3 = 0x00,
	AT45DB_READ_SECURITY_REGISTER_CMD = 0x77,

	//Additional commands
	AT45DB_MAIN_MEMORY_PAGE_TO_BUFFER_1_TRANSFER_CMD = 0x53,
	AT45DB_MAIN_MEMORY_PAGE_TO_BUFFER_2_TRANSFER_CMD = 0x55,
	AT45DB_MAIN_MEMORY_PAGE_TO_BUFFER_1_COMPARE_CMD = 0x60,
	AT45DB_MAIN_MEMORY_PAGE_TO_BUFFER_2_COMPARE_CMD = 0x61,
	AT45DB_AUTO_PAGE_REWRITE_THROUGH_BUFFER_1_CMD = 0x58,
	AT45DB_AUTO_PAGE_REWRITE_THROUGH_BUFFER_2_CMD = 0x59,
	AT45DB_DEEP_POWERDOWN_CMD = 0xb9,
	AT45DB_RESUME_FROM_DEEP_POWERDOWN_CMD = 0xab,
	AT45DB_STATUS_REGISTER_READ_CMD = 0xd7,
	AT45DB_MANUFACTURER_AND_DEVICE_ID_READ_CMD = 0x9F,
};

void at45BufWrite(void)
{
	volatile avr32_spi_t * spi = &AVR32_SPI1;
	int i;
	char writeThisData[] = "emCode";

	spiChipSelect(spi, CS0);

	//Buffer write cmd
	spiWriteByte(spi, AT45DB_BUFFER_1_WRITE_CMD);

	//Address, 24-bits
	spiWriteByte(spi, 0x00);
	spiWriteByte(spi, 0x00);
	spiWriteByte(spi, 0x00);

	//Data
	i = 0;
	while(writeThisData[i] != '\0') {
		spiWriteByte(spi, writeThisData[i]);
		++i;
	}
	//Write the last \0
	//~ spiWriteByte(spi, writeThisData[i]);

	spiChipDeselect(spi);

	//Write last byte and finish
	//~ spiWriteLastByte(spi, 0xff);
}

void at45BufRead(void)
{
	volatile avr32_spi_t * spi = &AVR32_SPI1;
	unsigned int ch;
	int dontCare = 0xff;

	spiChipSelect(spi, CS0);

	//Buffer read cmd
	spiWriteByte(spi, AT45DB_BUFFER_1_READ_CMD);

	//Address
	spiWriteByte(spi, 0x00);
	spiWriteByte(spi, 0x00);
	spiWriteByte(spi, 0x00);

	//One additional dontCare byte to initialize the read cmd
	spiWriteByte(spi, dontCare);

	//Read buffer
	//~ do {
		spiWriteByte(spi, dontCare);
		spiReadByte(spi, &ch);
		usartWriteChar(USART0, ch);

	//} while(ch != 0xff);

	spiChipDeselect(spi);

}

int at45ReadDeviceID(volatile avr32_spi_t * spi)
{
	#define NR_OF_ID_BYTES 3
	int i;
	int dontCare = 0xff;
	unsigned int byte;
	int id = 0;

	spiChipSelect(spi, CS0);

	//Read Manufacturer and ID cmd
	spiWriteByte(spi, 0x9f);

	for(i = 0; i < NR_OF_ID_BYTES; ++i) {
		spiWriteByte(spi, dontCare);
		spiReadByte(spi, &byte);
		id <<= 8;
		id |= byte;
	}
	spiChipDeselect(spi);
	return id;
}

int at45ReadStatus(volatile avr32_spi_t * spi)
{
	unsigned int status;
	int dontCare = 0;
	spiWriteByte(spi, dontCare);
	spiReadByte(spi, &status);
	return status;
}

int at45WaitUntilReady(volatile avr32_spi_t * spi)
{
	int status;
	int timeout = 1000;
	//~ int dontCare = 0xff;

	spiChipSelect(spi, CS0);
	spiWriteByte(spi, 0xd7);
	do {
		//spiWriteByte(spi, dontCare);
		status = at45ReadStatus(spi);
		//spiReadByte(spi, &status);
		if(!timeout--) {
			spiChipDeselect(spi);
			usartWriteLine(USART0, (char *)"at45db642_SR timeout\n");	//TODO: remove
			return SPI_TIMEOUT;
		}
		status &= 0xff;
	} while((status & AT45DB642_SR_RDY) == 0);

	spiChipDeselect(spi);
	return SPI_OK;
}

void at45test(void)
{
	volatile avr32_spi_t * spi = &AVR32_SPI1;
	int id;

	//~ spiEnable(spi, AT45DB_CS, CONFIG_CS0_PIN, CONFIG_CS0_FUNCTION);
	spiSetup(spi, AT45DB_CS, AT45DB_CS_PIN, AT45DB_CS_FUNCTION);
	spiSetMode(AT45DB_SPI, AT45DB_CS);
	spiSetSpck(AT45DB_SPI, AT45DB_CS, 1000000);
	spiEnable(AT45DB_SPI);

	if(at45WaitUntilReady(spi) == SPI_TIMEOUT) {
		usartWriteLine(USART0, (char *)"at45WautUntilReady timeout\n");	//TODO: remove
	}

	//Read and display id
	id = at45ReadDeviceID(spi);
	usartWriteLine(USART0, (char *)"AT45DB ID: ");
	usartWriteValue(USART0, id);

	at45WaitUntilReady(spi);
	at45BufWrite();
	at45WaitUntilReady(spi);
	at45BufRead();
	at45WaitUntilReady(spi);
}