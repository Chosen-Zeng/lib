#ifndef __CRC_H
#define __CRC_H

#include <stdio.h>
#include <stdbool.h>

typedef const struct
{
    unsigned short poly, init, XorOut;
    bool RefIn, RefOut;
    const unsigned short *tab;
} CRC_16_t;

extern CRC_16_t CRC_16_XModem, CRC_16_CCITT, CRC_16_CCITT_FALSE;

// extern unsigned short CRC_16_tab[256];

void CRC_16_TabCalc(CRC_16_t *CRC_16);
unsigned short CRC_16_Cal(CRC_16_t *CRC_16, const unsigned char str[], size_t len);

#endif