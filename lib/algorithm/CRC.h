#ifndef __CRC_H
#define __CRC_H

#include <stdio.h>

typedef const struct
{
    unsigned short poly, init, XorOut;
    bool RefIn, RefOut;
} CRC_16_t;

extern CRC_16_t CRC_16_XModem, CRC_16_CCITT, CRC_16_CCITT_FALSE;

extern unsigned short CRC_16_tab[256];

unsigned short crc_ccitt(unsigned short crc, unsigned char const *buffer, size_t len);

void CRC_16_TabCalc(CRC_16_t *CRC_16);

#endif