
#include "drivers/spi.h"

#define SD_SPI (&AVR32_SPI1)
#define SD
#define DI_HIGH 0xff

enum {
		CMD0 = 0,
		CMD8 = 8,
		CMD58 = 58
};

DSTATUS disk_initialize(BYTE Drive)
{
	int i;

	void spiEnable(volatile avr32_spi_t * spi, const int cs, const int cs_pin, const int cs_function);
	spiEnable(SD_SPI, SD_CS, SD_CS_PIN, SD_CS_FUNCTION);

	//Wait atleast 1 ms for voltage to stabilize above 2.2 V
	delay_ms(2);

	//Set DI and CS high and
	//Wait at least 74 SPCK
	for(i = 0; i < 8; ++i) {
		spiWriteByte(SPI_SD, DI_HIGH);
	}

	//The SD-card is now ready to accept native commands

	//Software reset





}

DSTATUS disk_status (
  BYTE Drive     /* Physical drive number */
);

DRESULT disk_read (
  BYTE Drive,          /* Physical drive number */
  BYTE* Buffer,        /* Pointer to the read data buffer */
  DWORD SectorNumber,  /* Start sector number */
  BYTE SectorCount     /* Number of sectros to read */
);

DRESULT disk_write (
  BYTE Drive,          /* Physical drive number */
  const BYTE* Buffer,  /* Pointer to the write data (may be non aligned) */
  DWORD SectorNumber,  /* Sector number to write */
  BYTE SectorCount     /* Number of sectors to write */
);

DRESULT disk_ioctl (
  BYTE Drive,      /* Drive number */
  BYTE Command,    /* Control command code */
  void* Buffer     /* Parameter and data buffer */
);

DWORD get_fattime (void);
