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

#ifndef _ROSA_PM_H_
#define _ROSA_PM_H_

#include <avr32/pm.h>

enum {
	PM_CPUMASK_OCD_OFFSET = 1
}

enum {
	PM_PBAMASK_INTC_OFFSET = 0,
	PM_PBAMASK_GPIO_OFFSET,
	PM_PBAMASK_PDCA_OFFSET,
	PM_PBAMASK_PM_RTC_EIC_OFFSET,
	PM_PBAMASK_ADC_OFFSET,
	PM_PBAMASK_SPI0_OFFSET,
	PM_PBAMASK_SPI1_OFFSET,
	PM_PBAMASK_TWI_OFFSET,
	PM_PBAMASK_USART0_OFFSET,
	PM_PBAMASK_USART1_OFFSET,
	PM_PBAMASK_PWM_OFFSET,
	PM_PBAMASK_SSC_OFFSET
}

enum {
	PM_PBBMASK_HMATRIX_OFFSET = 0,
	PM_PBBMASK_USBB_OFFSET,
	PM_PBBMASK_FLASHC_OFFSET,
	PM_PBBMASK_MACB_OFFSET,
	PM_PBBMASK_SMC_OFFSET,
	PM_PBBMASK_SDRAMC_OFFSET,
}


//~ void pmCpuMaskSet(unsigned int mask);
//~ unsigned int pmCpuMaskGet(void);
void pmHsbMaskSet(unsigned int mask);
unsigned int pmHsbMaskGet(void);
void pmPbaMaskSet(unsigned int mask);
unsigned int pmPbaMaskGet(void);
void pmPbbMaskSet(unsigned int mask);
unsigned int pmPbbMaskGet(void);

#endif /* _ROSA_PM_H_ */