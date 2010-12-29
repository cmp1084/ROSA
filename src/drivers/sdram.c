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

#include <avr32/io.h>
#include "drivers/sdramc.h"
#include "drivers/gpio.h"
#include "drivers/delay.h"
//~ #include "drivers/usart.h"

#define SDRAM_BASE 0xd0000000

/*
{*
* Clock related bits
* {pin = 42 '*', function = 2 '\002'},
* {pin = 43 '+', function = 2 '\002'}}
*
* Control bits
* {pin = 44 ',', function = 2 '\002'},
* {pin = 45 '-', function = 2 '\002'},
* {pin = 46 '.', function = 2 '\002'},
*
* SDA
* {pin = 48 '0', function = 2 '\002'},
*
* Data mask bits (also pin 109)
* {pin = 70 'F', function = 0 '\0'},
*
* Addr bits
* {pin = 72 'H', function = 0 '\0'},
* {pin = 73 'I', function = 0 '\0'},
* {pin = 74 'J', function = 0 '\0'},
* {pin = 75 'K', function = 0 '\0'},
* {pin = 76 'L', function = 0 '\0'},
* {pin = 77 'M', function = 0 '\0'},
* {pin = 78 'N', function = 0 '\0'},
* {pin = 79 'O', function = 0 '\0'},
* {pin = 80 'P', function = 0 '\0'},
* {pin = 81 'Q', function = 0 '\0'},
* {pin = 83 'S', function = 0 '\0'},
* {pin = 84 'T', function = 0 '\0'},
*
* Bank address bits
* {pin = 86 'V', function = 0 '\0'},
* {pin = 87 'W', function = 0 '\0'},
*
* Data bits
* {pin = 90 'Z', function = 0 '\0'},
* {pin = 91 '[', function = 0 '\0'},
* {pin = 92 '\\', function = 0 '\0'},
* {pin = 93 ']', function = 0 '\0'},
* {pin = 94 '^', function = 0 '\0'},
* {pin = 95 '_', function = 0 '\0'},
* {pin = 96 '`', function = 0 '\0'},
* {pin = 97 'a', function = 0 '\0'},
* {pin = 98 'b', function = 0 '\0'},
* {pin = 99 'c', function = 0 '\0'},
* {pin = 100 'd', function = 0 '\0'},
* {pin = 101 'e', function = 0 '\0'},
* {pin = 102 'f', function = 0 '\0'},
* {pin = 103 'g', function = 0 '\0'},
* {pin = 104 'h', function = 0 '\0'},
* {pin = 105 'i', function = 0 '\0'},
*
* Control bit (together with 44, 45, 46)
* {pin = 106 'j', function = 0 '\0'},
*
* Data mask (also pin 70)
* {pin = 109 'm', function = 0 '\0'},
*/

//~
//~ int sysreg_read(void * sysreg)
//~ {
	//~ int r12;
	//~ asm("mfsr %0,66" : "=r"(r12));
	//~ return r12;
//~ }



/* From asm/sdram.h */
//~ struct sdram_config {
	//~ /* Number of data bits. */
	//~ enum {
		//~ SDRAM_DATA_16BIT = 16,
		//~ SDRAM_DATA_32BIT = 32,
	//~ } data_bits;
//~
	//~ /* Number of address bits */
	//~ uint8_t row_bits, col_bits, bank_bits;
//~
	//~ /* SDRAM timings in cycles */
	//~ uint8_t cas, twr, trc, trp, trcd, tras, txsr;
//~
	//~ /* SDRAM refresh period in cycles */
	//~ unsigned long refresh_period;
//~ };

/* From initsdram board/atmel/mizar.c (or similar) */
//~ static const struct sdram_config sdramconfig = {
	//~ /* 2x16MBit, MT48LC16M16A2TG7E */
	//~ .data_bits = 16,
	//~ .row_bits = 13,
	//~ .col_bits = 9,
	//~ .cas = 2,
	//~ .twr = 14,
	//~ .trc = 60,
	//~ .trp = 15,
	//~ .trcd = 15,
	//~ .tras = 37,
	//~ .txsr = 67,
//~ };

unsigned int sysreg_read(unsigned char sysreg)
{
	unsigned int r12;
	//~ asm("mfsr %0,%1" : "=r"(r12) : "a"(sysreg));
	asm("mfsr %0,264"  :  "=r"(r12));
	return r12;
}

#define COUNT 66
void sdramc_ns_delay(unsigned long ck)
{
  // Use the CPU cycle counter (CPU and HSB clocks are the same).
  unsigned long delay_start_cycle = (unsigned long)sysreg_read(COUNT);
  unsigned long delay_end_cycle = delay_start_cycle + ck;

  // To be safer, the end of wait is based on an inequality test, so CPU cycle
  // counter wrap around is checked.
  if (delay_start_cycle > delay_end_cycle)
  {
    while ((unsigned long)sysreg_read(COUNT) > delay_end_cycle);
  }
  while ((unsigned long)sysreg_read(COUNT) < delay_end_cycle);
}

#define SDRAM_TR 7812	//Maximum refresh time, Unit ns.
struct sdram_config {
	/* Number of data bits. */
	enum {
		SDRAM_DATA_16BIT = 16,	//Always 16-bit data on AVR32UC3A0512, 20100629 Mjan
		SDRAM_DATA_32BIT = 32,
	} data_bits;

	/* Number of address bits */
	unsigned char row_bits, col_bits, bank_bits;

	/* SDRAM timings in cycles */
	unsigned char cas, twr, trc, trp, trcd, tras, txsr;

	/* SDRAM refresh period in cycles */
	unsigned long refresh_period;
};

static const struct sdram_config sdramconfig = {
		/* MT48LC16M16A2TG7E */
		.data_bits = 16,
		.row_bits = 13,
		.col_bits = 9,
		.bank_bits = 2,
		.cas = 2,
		.twr = 14,
		.trc = 60,
		.trp = 15,
		.trcd = 15,
		.txsr = 67,
		.tras = 37,
		.refresh_period = 7812 * 12 / 1000,
		//~ //.refresh_period = (781 * (SDRAMC_BUS_HZ / 1000)) / 100000,  //depends on PLL
		//~ /* NGW100, same MT48LC16M16A SDRAM
		//~ .data_bits      = SDRAM_DATA_16BIT,
		//~ .row_bits = 13,
		//~ .col_bits = 9,
		//~ .bank_bits = 2,
		//~ .cas = 3,
		//~ .twr = 2,
		//~ .trc = 7,
		//~ .trp = 2,
		//~ .trcd = 2,
		//~ .tras = 5,
		//~ .txsr = 5,
		//~ */
};

unsigned long sdramInit(void)
{
	volatile avr32_hmatrix_t * hmatrix = (void *) 0xfffe1000;
	volatile avr32_sdramc_t * sdramc = (void *) 0xfffe2000;
	void * sdram_base = (void *) 0xd0000000;
	const struct sdram_config * config = &sdramconfig;

	unsigned int sdram_size;
	unsigned int cfgreg;
	unsigned long hsb_hz;
	unsigned long hsb_mhz_dn;
	unsigned long hsb_mhz_up;
	volatile unsigned long * sdram;
	unsigned long i;
	unsigned int dummyread;

	gpioPeripheralEnableMultiPin(PORTMUX_PORT_B,
		(1 << (42-32)) |
		(1 << (43-32)) |
		(1 << (44-32)) |
		(1 << (45-32)) |
		(1 << (46-32)) |
		(1 << (48-32)),
		FUNC_C);


	gpioPeripheralEnableMultiPin(PORTMUX_PORT_C,
		(1 << (70-64)) |
		(1 << (72-64)) |
		(1 << (73-64)) |
		(1 << (74-64)) |
		(1 << (75-64)) |
		(1 << (76-64)) |
		(1 << (77-64)) |
		(1 << (78-64)) |
		(1 << (79-64)) |
		(1 << (80-64)) |
		(1 << (81-64)) |
		(1 << (82-64)) |
		(1 << (83-64)) |
		(1 << (84-64)) |
		(1 << (86-64)) |
		(1 << (87-64)) |
		(1 << (90-64)) |
		(1 << (91-64)) |
		(1 << (92-64)) |
		(1 << (93-64)) |
		(1 << (94-64)) |
		(1 << (95-64)),
		FUNC_A);

	gpioPeripheralEnableMultiPin(PORTMUX_PORT_X,
		(1 << (96 - 96)) |
		(1 << (97 - 96)) |
		(1 << (98 - 96)) |
		(1 << (99 - 96)) |
		(1 << (100 - 96)) |
		(1 << (101 - 96)) |
		(1 << (102 - 96)) |
		(1 << (103 - 96)) |
		(1 << (104 - 96)) |
		(1 << (105 - 96)) |
		(1 << (106 - 96)) |
		(1 << (109 - 96)),
		FUNC_A);

	sdram = (void *) 0xd0000000;			//TODO: Same as sdram_base, use that instead.
	hsb_hz = FOSC0;							//get_hsb_clk_rate();	//was: CFG_OSC0_HZ; 20100716 Mjan
	hsb_mhz_dn = hsb_hz / 1000000;
	hsb_mhz_up = (hsb_hz + 999999) / 1000000;


	//Enable SDRAM mode for CS1
	//~ hmatrix_write(SFR[5], hmatrix_read(SFR[5] | (1 << 1)));
	//~ hmatrix_read(SFR[5]);
	hmatrix->sfr[5] |= 1 << 1;
	dummyread = hmatrix->sfr[5];

	cfgreg = 0x35ffced9;	//Lets optimize by hand.
	//~ cfgreg = (SDRAMC_BF(NC, config->col_bits - 8)
				//~ | SDRAMC_BF(NR, config->row_bits - 11)
				//~ | SDRAMC_BF(NB, config->bank_bits - 1)
				//~ | SDRAMC_BF(CAS, config->cas)
				//~ | SDRAMC_BF(TWR, config->twr)
				//~ | SDRAMC_BF(TRC, config->trc)
				//~ | SDRAMC_BF(TRP, config->trp)
				//~ | SDRAMC_BF(TRCD, config->trcd)
				//~ | SDRAMC_BF(TRAS, config->tras)
				//~ | SDRAMC_BF(TXSR, config->txsr));
	//~ if(config->data_bits == SDRAM_DATA_16BIT) {
		//~ cfgreg |= SDRAMC_BIT(DBW);
	//~ }

	//~ cfgreg = 0x111111d9;	//Snatched from AVR32 Studio
	/* Interrupts should be turned off at this point */
	//~ sdramc_writel(CR, cfgreg);
	//~ sdramc_readl(CR);
	sdramc->cr = cfgreg;
	dummyread = sdramc->cr;

	/* Set the Memory Device Register to SDRAM */
	//~ sdramc_writel(MDR, SDRAMC_MDR_SDRAM);
	sdramc->mdr = SDRAMC_MDR_SDRAM;

	/* Send a NOP to turn on the clock (necessary on some chips) */
	//~ sdramc_writel(MR, SDRAMC_MODE_NOP);
	//~ sdramc_readl(MR);
	//~ readl(sdram_base);
	sdramc->mr = SDRAMC_MODE_NOP;
	dummyread = sdramc->mr;
	dummyread = *(unsigned int *)sdram_base;

	/*
	 * Initialization sequence for SDRAM, from the data sheet:
	 *
	 * 1. A minimum pause of 200 us is provided to precede any
	 *    signal toggle.
	 */
	delay_us(200);	//Not really worth optimizing at this point...
	//~ udelay(200);

	/*
	 * 2. A Precharge All command is issued to the SDRAM
	 */
	//~ sdramc_writel(MR, SDRAMC_MODE_BANKS_PRECHARGE);
	//~ sdramc_readl(MR);
	//~ readl(sdram_base);
	//~ sdramc_ns_delay(((15) * (hsb_mhz_up) + 999) / 1000);
	sdramc->mr = SDRAMC_MODE_BANKS_PRECHARGE;
	dummyread = sdramc->mr;
	dummyread = *(unsigned int *)sdram_base;
	sdramc_ns_delay(((15) * (hsb_mhz_up) + 999) / 1000);

	/*
	 * 3. Eight auto-refresh (CBR) cycles are provided, (only two should be necessary)
	 */
	//~ sdramc_writel(MR, SDRAMC_MODE_AUTO_REFRESH);
	//~ sdramc_readl(MR);
	//~ for(i = 0; i < 8; i++) {	//Was: i < 8; 20100629 Mjan
		//~ readl(sdram_base);
		//~ sdramc_ns_delay(((66) * (hsb_mhz_up) + 999) / 1000);
	//~ }
	sdramc->mr = SDRAMC_MODE_AUTO_REFRESH;
	dummyread = sdramc->mr;
	for(i = 0; i < 8; i++) {
		dummyread = *(unsigned int *)sdram_base;
		sdramc_ns_delay(((66) * (hsb_mhz_up) + 999) / 1000);
	}

	/*
	 * 4. A mode register set (MRS) cycle is issued to program
	 *    SDRAM parameters, in particular CAS latency and burst
	 *    length.
	 *
	 * The address will be chosen by the SDRAMC automatically; we
	 * just have to make sure BA[1:0] are set to 0.
	 */
	//~ sdramc_writel(MR, SDRAMC_MODE_LOAD_MODE);
	//~ sdramc_readl(MR);
	//~ readl(sdram_base);
	//~ sdramc_ns_delay(((2) * (hsb_mhz_up) + 999) / 1000);

	sdramc->mr = SDRAMC_MODE_LOAD_MODE;
	dummyread = sdramc->mr;
	dummyread = *(unsigned int *)sdram_base;
	sdramc_ns_delay(((2) * (hsb_mhz_up) + 999) / 1000);

	/*
	 * 5. The application must go into Normal Mode, setting Mode
	 *    to 0 in the Mode Register and performing a write access
	 *    at any location in the SDRAM.
	 */
	//~ sdramc_writel(MR, SDRAMC_MODE_NORMAL);
	//~ sdramc_readl(MR);
	//~ readl(sdram_base);

	sdramc->mr = SDRAMC_MODE_NORMAL;
	dummyread = sdramc->mr;
	dummyread = *(unsigned int *)sdram_base;

	/*
	 * 6. Write refresh rate into SDRAMC refresh timer count
	 *    register (refresh rate = timing between refresh cycles).
	 */
	//This value should be 0x5d, at least AVR32 studio example show this value.
	//~ sdramc_writel(TR, (SDRAM_TR * hsb_mhz_dn) / 1000); //config->refresh_period);
	//~ sdramc_readl(TR);

	sdramc->tr = (SDRAM_TR * hsb_mhz_dn) / 1000;
	dummyread = sdramc->tr;

	//~ if (config->data_bits == SDRAM_DATA_16BIT)
		sdram_size = 1 << (config->row_bits + config->col_bits + config->bank_bits + 1);
	//~ else
		//~ sdram_size = 1 << (config->row_bits + config->col_bits + config->bank_bits + 2);
	return sdram_size;
}
