#include "USART.h"
#include "TIM.h"

inline void UART_SendData(USART_TypeDef *USART_handle, unsigned char TxData[], unsigned char len, float timeout)
{
    timer_t USART_time = timer_InitStruct;
    unsigned char cnt = 0;
    while (TIMSW_TimeLimit(&USART_time, timeout) && cnt < len)
    {
        if (USART_handle->ISR & 0x80) // TXE
            USART_handle->TDR = TxData[cnt++];
    }
}

inline void UART_SendData_DMA(DMA_Stream_TypeDef *DMA_Stream_handle, USART_TypeDef *USART_handle, unsigned char TxData[], unsigned char len)
{
    // clear flags if uneffective
    // do
    // {
    //     DMA1->LIFCR |= 0x3D;
    // } while (DMA1->LISR & 0x3D);

    DMA_Stream_handle->NDTR = len;
    DMA_Stream_handle->PAR = (unsigned)&USART_handle->TDR;
    DMA_Stream_handle->M0AR = (unsigned)TxData;
    DMA_Stream_handle->CR = 0x00010441;
}

// void USART3_IRQHandler(void)
// {
//     static unsigned char RxData[17], cnt;
//     if (USART3->ISR & 0x20)
//     {
//         RxData[cnt++] = USART3->RDR;
//     }
//     else if (USART3->ISR & 0x10)
//     {
//         USART3->ICR |= 0x10;
//         cnt = 0;
//     }
//     else if (USART3->ISR & 0x80) // overrun
//     {
//         USART3->ICR |= 0x80;
//     }
// }