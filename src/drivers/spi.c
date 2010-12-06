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
void spiEnable(volatile avr32_spi_t * spi)
{
	//Errata:
	//41.2.3
	//do not set SPI FDIV = 1
	//Bad serial clock on 2nd chip select SCBR = 1, CPOL = 1, NCPHA = 0
	//


	//Enable SPI1 clocking
	pmPbaMaskSet((1 << PM_PBAMASK_SPI1_OFFSET) | pmPbaMaskGet());

	//Enable SPI1 peripheral pins
	gpioPeripheralEnable(CONFIG_MISO, CONFIG_MISO_FUNCTION);
	gpioPeripheralEnable(CONFIG_MOSI, CONFIG_MOSI_FUNCTION);
	gpioPeripheralEnable(CONFIG_SCK, CONFIG_SCK_FUNCTION);
	gpioPeripheralEnable(CONFIG_CS, CONFIG_CS_FUNCTION);

	//Enable and SW reset SPI
	spi->cr = 1 << AVR32_SPI_SPIEN_OFFSET;
	//spi->cr = (1 << AVR32_SPI_SWRST) | (1 << AVR32_SPI_SPIEN);

	//Set master mode
	spi->mr |= (1 << AVR32_SPI_MSTR_OFFSET);

	//Loopback during testing
	spi->mr |= (1 << AVR32_SPI_LLB_OFFSET);	//TODO: Only during test

	//Disable mode fault detection
	spi->mr |= (1 << AVR32_SPI_MODFDIS_OFFSET);

	//Set polarity
	//Set phase
	//Set baudrate, 0 is forbidden
	spi->csr0 |= (0x7f << AVR32_SPI_CSR0_SCBR_OFFSET) | (1 << AVR32_SPI_CSR0_CSAAT_OFFSET);

	//Set CSAAT, chip select active after transfer
	//Select chip

	//Testdrive
	volatile int sr, cs  = 1;
	volatile int i, j;
	int timeout;

	//double dummy read
	j = spi->rdr;
	i = spi->rdr;

	for(i = 0; i < 10; ++i) {
		spi->tdr = (cs << AVR32_SPI_PCS_OFFSET) | i;

		timeout = 4000;
		while(!(spi->sr & (1 << AVR32_SPI_SR_TXEMPTY_OFFSET)))
		{
			timeout--;
			if(timeout == 0) {
				ledOn(LED3_GPIO);
				break;
			}
		}

		j = spi->rdr;
		j &= 0xff;
		if(j != i) {
			ledOn(LED5_GPIO);
		}
		sr = spi->sr;
	}
}

