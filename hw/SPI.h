#ifndef __SPI_H
#define __SPI_H

#include "user.h"

static inline unsigned char SPI_SendData(SPI_TypeDef *SPI_handle, unsigned short data)
{
    SPI_handle->CR1 |= 0x40; // enable SPI for STM32G4
    // SPI_handle->CR1 |= 0x200; // STM32H7 series
    if (SPI_handle->SR & 0x2)
    {
        SPI_handle->DR = data; // DR for STM32G4
        return 0;
    }
    return 1;
}

#endif