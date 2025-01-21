#include "USART.h"
#include "TIM.h"

inline void UART_SendData(USART_TypeDef *USART_handler, unsigned char data[], unsigned char len, float timeout)
{
    timer_t USART_time = timer_InitStruct;
    unsigned char cnt = 0;
    while (TIMSW_TimeLimit(&USART_time, timeout) && cnt < len)
    {
        if (USART_handler->ISR & 0x80) // TXE
            USART_handler->TDR = data[cnt++];
    }
}