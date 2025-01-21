#include "AS5047P.h"

unsigned char ParityCalc(unsigned short data)
{
    unsigned char cnt = 15, ret = 0;
    while (cnt--)
    {
        ret ^= data >> cnt;
    }

    return ret & 1;
}

unsigned char AS5047P_ReadData(SPI_TypeDef *SPI_handle, unsigned short AS5047P_reg)
{
    return SPI_SendData(SPI_handle, ParityCalc(AS5047P_R | AS5047P_reg) << 15 | AS5047P_R | AS5047P_reg);
}

// void SPI5_IRQHandler(void)
//{
// }