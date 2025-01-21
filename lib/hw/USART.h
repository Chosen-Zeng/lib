#ifndef __USART_H
#define __USART_H

#include "user.h"

inline void UART_SendData(USART_TypeDef *UART_handler, unsigned char data[], unsigned char len, unsigned char timeout);

#endif