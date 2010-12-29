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
/*
¤ø„¸   ¨°º¤ø„¸¸„ø¤º°¨¸„ø¤º°¨
¨°º¤ø„¸  emCode.se ¸„ø¤º°¨
¸„ø¤º°¨we do it hard!¨°º¤ø„¸
¸„ø¤º°¨¸„ø¤º°¨¨°º¤ø„¸¨°º¤ø„
*/

//Hardware dependent code below
#include <avr32/io.h>

#include "drivers/sdhc.h"
#include "drivers/delay.h"

//Only during debugging
#include "drivers/usart.h"
#include "drivers/led.h"


unsigned char sdhcbuf[512+16];
unsigned char * filesectbuf = &sdhcbuf[4];

void sendspiblock(unsigned char * buf, unsigned short len)
{
	ledOn(LED0_GPIO);
	while(len--) {
		spiWriteByte(SD_SPI, *buf++);
		//asm(" .rept 9\n nop\n .endr");
	}
	ledOff(LED0_GPIO);
}


static void recvspiblock(unsigned short len)
{
	unsigned char * buf = filesectbuf;
	unsigned int byte;
	if(!len) {
		return;
	}

	//~ spiWriteByte(SD_SPI, 0xff);
	//~ spiReadByte(SD_SPI, &byte);
	//~ *buf = (char)byte;
	//~ buf++;
	//~ usartWriteValue(USART0, byte);
	//~ usartWriteLine(USART0, "\n");
	while(--len) {
		//asm(" .rept 9\n nop\n .endr");
		spiWriteByte(SD_SPI, 0xff);
		spiReadByte(SD_SPI, &byte);	//double buffered
		*buf = (char)byte;
		buf++;
		//usartWriteValue(USART0, byte);
		//usartWriteLine(USART0, "\n");
	}
	//~ while(!(spiReadSR() & AVR32_SPI_SR_TXEMPTY_MASK));
	//~ spiReadByte(SD_SPI, &byte);
	//~ *buf = byte;
	//~ usartWriteValue(USART0, byte);
	//~ usartWriteLine(USART0, "\n");
}


static void sendffspi(unsigned short len)
{
	while(len--) {
		spiWriteByte(SD_SPI, 0xff);
	}
}


static void waitforspi(unsigned char val)
{
	unsigned int byte;
	while(1) {
		spiWriteByte(SD_SPI, 0xff);
		spiReadByte(SD_SPI, &byte);
		if(byte == val) {
			return;
		}
		//TODO: Timeout
	}
}


static unsigned char waitnotspi(unsigned char val)
{
	unsigned char timeout = 8;	//8 from SD specification
	unsigned int got;
	//usartWriteLine(USART0, "waitnotspi(): ");
	while(timeout--) {
		spiWriteByte(SD_SPI, 0xff);
		spiReadByte(SD_SPI, &got);
		//~ usartWriteValue(USART0, got);
		//~ usartWriteLine(USART0, " ");
		if(val != (char)got) {
		//	usartWriteLine(USART0, "\n");
			return got;
		}
	}
	//usartWriteLine(USART0, "timeout, ");
	return got;
}

static void csact(void)
{
	spiChipSelect(SD_SPI, SD_CS);
	sendffspi(1);
}

static void csinact(void)
{
	spiChipDeselect(SD_SPI);
	sendffspi(1);
}

//~ static void spihwinit(void)
//~ {
	//~ int drive = 0;
	//~ disk_initialize(drive);
//~ }


//END HARDWARE DEPENDENT
#define SDECHO 0xaa
#define RECVBUFOFFSET 1
static unsigned char goidle[6] = { 0+0x40, 0, 0, 0, 0, 0x95 }; // R1
static unsigned char sendifcond[6] = { 8+0x40, 0, 0, 1, SDECHO, 0x87 }; //TODO: Autocalc CRC and send random echo value each time
static unsigned char readocr[6] = { 58+0x40, 0, 0, 0, 0, 0xff }; // R3
static unsigned char doapp[6] = { 55+0x40, 0, 0, 0, 0, 0xff }; // R1
static unsigned char sendopcond[6] = { 41+0x40, 0x40, 0, 0, 0, 0xff }; // HCS, R1 - activate init

static unsigned char ishccard;

// send command to SPI card, wait for response
static unsigned char sendcommand(unsigned char * cmd)
{
    sendffspi(1);        // needed to insure card is ready
    sendspiblock(cmd, 6);
    return waitnotspi(0xff); // return first status byte
}

static unsigned char sdhcinitseq(void)
{
    unsigned char needretry, nov2card;
    unsigned short i;

#define MAXTRIES1 50
    for (i = 0; i < MAXTRIES1; ++i) {
		//~ usartWriteLine(USART0, "GO_IDLE\n");
        if (1 == sendcommand(goidle)) { //R1 idle bit
            break;
		}
	}
    if (i >= MAXTRIES1) {
        return 1;
	}

    /* check for version of SD card specification */
    //~ usartWriteLine(USART0, "R1 SEND_IF_COND (CMD8): ");
    i = sendcommand(sendifcond);
    nov2card = 0;
//    usartWriteValue(USART0, i);
    usartWriteLine(USART0, "\n");
    if (!(4 & i)) {             //legal command - 2.0 spec
        recvspiblock(5);
		//~ usartWriteLine(USART0, "if(!(4 & i)) { recvspiblock(5)...\n");
        if (!(filesectbuf[1+RECVBUFOFFSET] & 0x01)      //Voltage accepted?
          || (filesectbuf[2+RECVBUFOFFSET] != SDECHO))  /* test pattern echoed back correctly */
            return 2;
    } else {
        nov2card = 1;
		//~ usartWriteLine(USART0, "v2 card\n");
	}

    // technically optional
    ishccard = 0;
    needretry = 0;
    //~ usartWriteLine(USART0, "READ_OCR\n");
    i = sendcommand(readocr);
    // should verify not illegal command, if so, it is not this kind of card
    //~ usartWriteLine(USART0, "recvspiblock(5)\n");
    recvspiblock(5);
    // Should verify voltage range here, bit 23 is 3.5-3.6, .1v lower going down
    // maybe later wait until not busy, but get the sdhc flag here if set
    if (filesectbuf[0] & 0x80) {
		ishccard = (filesectbuf[0] & 0x40);     // has CCS
		//~ usartWriteLine(USART0, "is sdhc card: ");
		//~ usartWriteValue(USART0, filesectbuf[1]);
		//~ usartWriteLine(USART0, "\n");
	}
    else {
		//~ usartWriteLine(USART0, "needretry = 1\n");
        needretry = 1;
	}

    // most take only a few loops if already inited
#define MAXTRIES2 30000
    for (i = 0; i < MAXTRIES2; ++i) {
        doapp[1] = ishccard;
        //~ usartWriteLine(USART0, "CMD55\n");
        sendcommand(doapp);     // not for MMC or pre-1, but ignored
        //~ usartWriteLine(USART0, "CMD41\n");
        if (!(1 & sendcommand(sendopcond)))
            break;
    }
    if (i >= MAXTRIES2)
        return 3;
    while(needretry && i++ < MAXTRIES2) {      // card was busy so CCS not valid
		//~ usartWriteLine(USART0, "read ocr, again\n");
        i = sendcommand(readocr);               // 0x3f, then 7 more	//UUUUUUh, wtf 'i'?!!1
        recvspiblock(5);
        ishccard = (filesectbuf[0] & 0x40);     // has CCS
        needretry = !(filesectbuf[0] & 0x80);	//This is such ugly code!
    }

    //~ usartWriteLine(USART0, "is sdhc card: ");
	usartWriteValue(USART0, filesectbuf[1]);
	//~ usartWriteLine(USART0, "\n");

    if (i >= MAXTRIES2)
        return 4;
    if (!ishccard) {            // set block to 512 for plain SD card
        unsigned char setblocklen[6] = { 16+0x40, 0, 0, 2, 0, 0xff };
        sendcommand(setblocklen);
        //~ usartWriteLine(USART0, "Not SDHC, setting blocklen=512 for SD card\n");
    }
    return 0;
}

static unsigned char getinfo[6] = { 9+0x40, 0, 0, 0, 0, 0xff };
unsigned int sdnumsectors = 0;

unsigned int cardsize(void)
{
    getinfo[0] = 9+0x40;   //CSD
    if (!sendcommand(getinfo)) {
        waitforspi(0xfe);
        recvspiblock(18);
    }
    else
        return 0;
    // Note: fsb[3] will contain max trans speed to set the SPI,
    // All SD memory cards will be 0x32 or 0x5a for 25Mhz or 50Mhz.
    // SDIO or older MMC may be slower.
    // Low 3 bits are bits per sec 100k * 10 ** bits (up to 100M)
    // bits 6:3 are multiplier:
    // RSV,1,1.2,1.3, 1.5,2.2.5,3, 3.5,4,4.5,5, 5.5,6,7,8
    unsigned long size;
    if( filesectbuf[0] & 0x40 ) {
        size = filesectbuf[7] & 0x3f;
        size <<= 16;
        size |= filesectbuf[8] << 8;
        size |= filesectbuf[9];
        ++size;
        return size << 10;
    }
    else  {
        size = (filesectbuf[6] & 3 ) << 10;
        size |= filesectbuf[7] << 2;
        size |= filesectbuf[8] >> 6;
        size++;
        unsigned mult;
        mult = ( filesectbuf[9] & 3 ) << 1;
        mult |= filesectbuf[10] >> 7;
        mult += 2 + (filesectbuf[5] & 0xf);
        size <<= mult - 9;
        return size;
    }
}

//Current Value Running
unsigned char sdhcinit()
{
	unsigned char i;
	//Wait atleast 1 ms for voltage to stabilize above 2.2 V
	delay_ms(1);
	csinact();                  //TODO: Unnecessaary
	sendffspi(10);              //SD cards need at least 74 clocks to start properly
	csact();
	i = sdhcinitseq();

	//~ usartWriteLine(USART0, "sdhcinitseq rc: ");
	//~ usartWriteValue(USART0, i);
	//~ usartWriteLine(USART0, "\n");

	sdnumsectors = cardsize();
	//~ usartWriteLine(USART0, "CardSize: ");
	//~ usartWriteValue(USART0, sdnumsectors);
	//~ usartWriteLine(USART0, "\n");

	csinact();
	return i;
}

unsigned char cardinfo(unsigned char which)
{
    unsigned char ret = 1;
    csact();
    getinfo[0] = 0x40 + (which ? 10 : 9);   //CID:CSD
    if (!sendcommand(getinfo)) {
        waitforspi(0xfe);
        recvspiblock(18);
        ret = 0;
    }
    csinact();
    return ret;
}

static unsigned char rw1bcmd[6];

static void setblockaddr(unsigned int blkaddr)
{
    if (!ishccard)
        blkaddr <<= 9;
    rw1bcmd[1] = blkaddr >> 24;
    rw1bcmd[2] = blkaddr >> 16;
    rw1bcmd[3] = blkaddr >> 8;
    rw1bcmd[4] = blkaddr;
    rw1bcmd[5] = 0xff;
}

unsigned char readsec(unsigned int blkaddr)
{
    unsigned char ret = 1;
    rw1bcmd[0] = 0x51;          // read
    setblockaddr(blkaddr);
    csact();
    if (!sendcommand(rw1bcmd)) {
        waitforspi(0xfe);
        recvspiblock(514);
        // CRC16 appended H,L
        ret = 0;
    }
    csinact();
    return ret;
}

unsigned char writesec(unsigned int blkaddr)
{
    unsigned char ret = 1;
    rw1bcmd[0] = 0x58;          // write
    setblockaddr(blkaddr);
    csact();
    if (!sendcommand(rw1bcmd)) {
        filesectbuf[-1] = 0xfe;
        // CRC
        filesectbuf[512] = 0xff; // CRCH
        filesectbuf[513] = 0xff; // CRCL
        sendspiblock(&filesectbuf[-1], 515);
        sendffspi(20); // wait for busy!
        waitforspi(0xff); // wait for not busy - equiv while == 0
        ret = 0;
    }
    csinact();
    return ret;
}

