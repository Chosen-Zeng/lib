#ifndef __USART_H
#define __USART_H

#include "user.h"

unsigned char UART_SendData(USART_TypeDef *USART_handle, unsigned char TxData);
unsigned char UART_SendStr(USART_TypeDef *USART_handle, unsigned char data[], unsigned char len, float timeout);
void UART_SendStr_DMA(DMA_Stream_TypeDef *DMA_Stream_handle, USART_TypeDef *USART_handle, unsigned char TxData[], unsigned char len);

#endif