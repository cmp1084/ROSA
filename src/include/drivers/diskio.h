


DSTATUS disk_initialize(BYTE Drive);           /* Physical drive number */

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
