#include "USART.h"
#include "TIM.h"

inline void UART_SendData(USART_TypeDef *UART_handler, unsigned char data[], unsigned char len, unsigned char timeout)
{
    timer_t UART_time = timer_InitStruct;
    unsigned char cnt = 0;
    while (TIMSW_TimeLimit(&UART_time, timeout) && cnt < len)
    {
        if (UART_handler->ISR & 0x80) // TXE
            UART_handler->TDR = data[cnt++];
    }
}