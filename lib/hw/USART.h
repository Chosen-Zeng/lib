#ifndef __USART_H
#define __USART_H

#include "user.h"

void UART_SendData(USART_TypeDef *USART_handler, unsigned char data[], unsigned char len, float timeout);

#endif