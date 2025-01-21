#ifndef __SPI_H
#define __SPI_H

#include "user.h"

unsigned char SPI_SendData(SPI_TypeDef *SPI_handle, unsigned short data);

#endif