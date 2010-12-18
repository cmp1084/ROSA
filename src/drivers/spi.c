/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// ///''//,
             ,///' '///,'/////',/////'  /////'\\,

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

#include "drivers/spi.h"
#include "drivers/led.h"

#ifdef CONFIG_SPI_TIMEOUTVALUE
#define SPI_TIMEOUTVALUE CONFIG_SPI_TIMEOUTVALUE
#else
#define SPI_TIMEOUTVALUE 10000
#endif

int spiCalcBaudDivisor(int cs, unsigned int spck, unsigned int mck);

//Caution, this give pessimistic baudrate.
int spiCalcBaudDivisor(int cs, unsigned int spck, unsigned int mck)
{
	int scbr;
	return scbr = (mck + spck / 2) / spck;
	//(0 is forbidden, 1 is fishy(?), anything else <256 is ok)
	//~ switch(scbr) {
		//~ case 0:
			//~ while(1);
			//~ break;
		//~ default:
			//~ return scbr;
			//~ break;
	//~ }
}

void spiSetSpck(volatile avr32_spi_t * spi, const int cs, const int spck)
{
	//This need to be fixed.
	int scbr;
	void * csrx;
	scbr = spiCalcBaudDivisor(cs, spck, FOSC0);
	scbr <<= AVR32_SPI_CSR0_SCBR_OFFSET;

	//Calculate the correct CSRx to write the spck to
	csrx = (void *)&(spi->csr0);
	csrx += 4 * cs;
	*(int *)csrx |= scbr;

	//~ switch(cs) {
		//~ case 0:
			//~ spi->csr0 |= scbr;
			//~ break;
		//~ case 1:
			//~ spi->csr1 |= scbr;
			//~ break;
		//~ case 2:
			//~ spi->csr2 |= scbr;
			//~ break;
		//~ case 3:
			//~ spi->csr3 |= scbr;
			//~ break;
		//~ }
}

void spiSetMode(volatile avr32_spi_t * spi, const int cs)
{
	int modeValue;
	void * csrx;

	//Set polarity
	//Set phase
	//Set SPI clock speed
	//~ spiSetSpck(spi, cs, spck);

	//Set CSAAT, chip select active after transfer
	modeValue = (1 << AVR32_SPI_NCPHA_OFFSET) | \
	            (1 << AVR32_SPI_CSR0_CSAAT_OFFSET);

	//Calculate the CSRx to use and write the modeValue
	csrx = (void *)&(spi->csr0);
	csrx += 4 * cs;
	*(int *)csrx |= modeValue;

	//~ *(int *)((void *)&(spi->csr0) + 4 * cs) |= modeValue;

	//~ switch(cs) {
		//~ case 0:
			//~ spi->csr0 |= modeValue;
			//~ break;
		//~ case 1:
			//~ spi->csr1 |= modeValue;
			//~ break;
		//~ case 2:
			//~ spi->csr2 |= modeValue;
			//~ break;
		//~ case 3:
			//~ spi->csr3 |= modeValue;
			//~ break;
		//~ default:
			//~ while(1);	//error, what were you thinking?!
	//~ }
}

void spiSetup(volatile avr32_spi_t * spi, const int cs, const int cs_pin, const int cs_function)
{
	//Errata:
	//41.2.3
	//do not set SPI FDIV = 1
	//Bad serial clock on 2nd chip select SCBR = 1, CPOL = 1, NCPHA = 0

	//Enable SPI1 clocking
	pmPbaMaskSet((1 << PM_PBAMASK_SPI1_OFFSET) | pmPbaMaskGet());

	//Enable SPI1 peripheral pins	//TODO: Fix, send CONFIG_MISO_ etc in some struct like avr32 framework does, it is actually the prettier solution :(
	gpioPeripheralEnable(CONFIG_MISO_PIN, CONFIG_MISO_FUNCTION);
	gpioPeripheralEnable(CONFIG_MOSI_PIN, CONFIG_MOSI_FUNCTION);
	gpioPeripheralEnable(CONFIG_SCK_PIN, CONFIG_SCK_FUNCTION);
	gpioPeripheralEnable(cs_pin, cs_function);

	//SW reset SPI
	spi->cr = 1 << AVR32_SPI_SWRST;

	//Set master mode
	//Disable mode fault detection
	//Deselect chip
	spi->mr = (1 << AVR32_SPI_MSTR_OFFSET) |    \
	          (1 << AVR32_SPI_MODFDIS_OFFSET) | \
	          (0x0f << AVR32_SPI_PCS_OFFSET);

	//spiSetMode(spi, cs, spck); done separately
}

void spiEnable(volatile avr32_spi_t * spi)
{
	//Enable SPI
	spi->cr = 1 << AVR32_SPI_SPIEN_OFFSET;
}


void spiLLBSet(volatile avr32_spi_t * spi, const int onoff)
{
	switch(onoff) {
		case ON:
			spi->mr |= 1 << AVR32_SPI_MR_LLB_OFFSET;
			break;
		default:
			spi->mr &= ~(1 << AVR32_SPI_MR_LLB_OFFSET);
			break;
	}
}

//input cs - Chip Select, legal values: CS0, CS1, CS2, CS3 (0, 1, 2, 3)
void spiChipSelect(volatile avr32_spi_t * spi, const int cs)
{
	spi->mr |= AVR32_SPI_MR_PCS_MASK & ~(1 << (AVR32_SPI_MR_PCS_OFFSET + cs));
	//This saves 12 bytes compared to the below.
	//~ spi->mr |= AVR32_SPI_MR_PCS_MASK;
	//~ spi->mr &= ~(1 << (AVR32_SPI_MR_PCS_OFFSET + cs));
}

int spiChipDeselect(volatile avr32_spi_t * spi)
{
	 int timeout = SPI_TIMEOUTVALUE;

	while(!(spi->sr & AVR32_SPI_SR_TXEMPTY_MASK)) {
		if (!timeout--) {
			return SPI_TIMEOUT;
		}
	}
	spi->mr |= AVR32_SPI_MR_PCS_MASK;
	spi->cr = AVR32_SPI_CR_LASTXFER_MASK;
	return SPI_OK;
}

int spiWriteByte(volatile avr32_spi_t * spi, const int byte)
{
	int timeout = SPI_TIMEOUTVALUE;

	//Is the transmit data register empty?
	while(!(spi->sr & AVR32_SPI_SR_TDRE_MASK)) {
		if(!timeout--) {
			return SPI_TIMEOUT;
		}
	}
	//Ok, previous byte have been sent. Send new byte.
	spi->tdr = byte;
	return SPI_OK;
}

int spiWriteLastByte(volatile avr32_spi_t * spi, const int byte)
{
	//Set the LASTXFER bit to deassert current NPCS after last transfer
	spi->cr = (1 << AVR32_SPI_LASTXFER_OFFSET);
	return spiWriteByte(spi, byte);
}

int spiReadByte(volatile avr32_spi_t * spi, unsigned int * byte)
{
	int timeout = SPI_TIMEOUTVALUE;

	while((spi->sr & (AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK)) != (AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK)) {
		if(!timeout--) {
			return SPI_TIMEOUT;
		}
	}
	*byte = spi->rdr & 0x000000ff;	//TODO: Constant
	return SPI_OK;
}