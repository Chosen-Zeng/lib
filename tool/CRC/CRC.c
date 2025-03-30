/*
    feature:
    - support up to 32 bit CRC calculation calculate by byte
    - output standard test result automatically

    use:
    - include .h alone to realize CRC function
    - compile and run .c to calculate CRC table
        argument for printf should change with specific CRC length
*/

#include <stdio.h>

#include "../../hw/CRC.h"

unsigned CRC_tab[256];

unsigned ReverseBits(unsigned data, unsigned char blen)
{
    unsigned reserve = data & 1;

    while (--blen)
    {
        reserve <<= 1;
        reserve |= (data >>= 1) & 1;
    }

    return reserve;
}

void CRC_TabCalc(CRC_info_t *CRC_info)
{
    unsigned char cnt = 0;

    do
    {
        unsigned CRC_val = cnt << CRC_info->len - 8;

        for (unsigned char cnt = 0; cnt < 8; cnt++)
            CRC_val = CRC_val & (1 << CRC_info->len - 1) ? (CRC_val << 1) ^ CRC_info->poly
                                                         : CRC_val << 1;

        CRC_tab[CRC_info->refin ? ReverseBits(cnt, 8) : cnt] = (CRC_info->refout ? ReverseBits(CRC_val, CRC_info->len) : CRC_val);

    } while (++cnt);
}

int main(void)
{
    CRC_TabCalc(&CRC_32_MPEG_2);

    for (unsigned char row = 0; row < 32; row++)
    {
        for (unsigned char column = 0; column < 8; column++)
        {
            printf("0x%08X", CRC_tab[row * 8 + column]); // change with specific CRC len

            if (row != 31 || column != 7)
                printf(", ");
        }
        printf("\n");
    }

    unsigned char test[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    printf("Test: 0x%08X", CRCsw_Calc(&CRC_32_MPEG_2, test, 9));
}