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

#include "drivers/delay.h"

// The following code produce a inner loop with six assembler instructions,
// corresponding to a 11 cycle loop, Which give approx. 12000000 / 1000 / 11 cycles per ms.
// Accuracy is low. We dont care about the fractions of 12000 / 11.
#define CYCLES_PER_MS (12000/11)
void delay_ms(unsigned int milliseconds)
{
	unsigned long cycles = (milliseconds * CYCLES_PER_MS);

	/* Busy-wait a number of clock cycles */
	while(cycles != 0) {
		cycles--;
	}

// The inner loop compile to this assembler if no optimization is used:
//~ 80002026:       ee f8 ff fc     ld.w    r8,r7[-4]       //1+1 cycles (stall)
//~ 8000202a:       20 18           sub     r8,1            //1   cycle
//~ 8000202c:       ef 48 ff fc     st.w    r7[-4],r8       //1+1 cycles (stall)
//~ 80002030:       ee f8 ff fc     ld.w    r8,r7[-4]       //1+1 cycles (stall)
//~ 80002034:       58 08           cp.w    r8,0            //1   cycle
//~ 80002036:       cf 81           brne    80002026        //1+2 cycle (stall)

}
