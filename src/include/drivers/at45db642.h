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
#ifndef __ROSA_AT45DB642_H_
#define __ROSA_AT45DB642_H_

#include <avr32/io.h>
#include "drivers/spi.h"
#include "drivers/spi.h"
#include "drivers/usart.h"
#include "rosa_config.h"

#define AT45DB_CS AVR32_SPI1_NPCS_0_0_PIN
void at45BufWrite(void);
void at45BufRead(void);

//Todo: Remove
void at45test(void);

#endif /* __ROSA_AT45DB642_H_ */