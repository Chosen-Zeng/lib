#include "SPI.h"

unsigned char SPI_SendData(SPI_TypeDef *SPI_handle, unsigned short data)
{
    SPI_handle->CR1 |= 0x200;
    if (SPI_handle->SR & 0x2)
    {
        SPI_handle->TXDR = data;
        return 0;
    }
    return 1;
}