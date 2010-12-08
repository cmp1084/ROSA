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

#ifndef __ROSA_SPI_H_
#define __ROSA_SPI_H_

#include <avr32/io.h>
#include "drivers/gpio.h"
#include "drivers/pm.h"
#include "rosa_config.h"
#include "kernel/rosa_def.h"

enum {
		SPI_TIMEOUT,
		SPI_ERROR,
		SPI_OK
};

enum {
		CS0 = 0,
		CS1,
		CS2,
		CS3
};

void spiEnable(volatile avr32_spi_t * spi, const int cs, const int cs_csfunction);
void spiLLBSet(volatile avr32_spi_t * spi, const int onoff);
void spiChipSelect(volatile avr32_spi_t * spi, const int cs);
int spiChipDeselect(volatile avr32_spi_t * spi);
int spiReadByte(volatile avr32_spi_t * spi, int * byte);
int spiWriteByte(volatile avr32_spi_t * spi, const int byte);
int spiWriteLastByte(volatile avr32_spi_t * spi, const int byte);



#endif /* __ROSA_SPI_H_ */