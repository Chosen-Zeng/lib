#include "SPI.h"

uint16_t test;
unsigned char SPI_SendData(SPI_TypeDef *SPI_handle, unsigned short data)
{
	test = data;
    SPI_handle->CR1 |= 0x40; // enable SPI for STM32G4
    // SPI_handle->CR1 |= 0x200; // STM32H7 series
    if (SPI_handle->SR & 0x2)
    {
        SPI_handle->DR = data;
        return 0;
    }
    return 1;
}