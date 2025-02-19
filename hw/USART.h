#ifndef __USART_H
#define __USART_H

#include "user.h"
#include "TIM.h"
#include "CRC.h"

static inline unsigned char UART_SendData(USART_TypeDef *USART_handle, unsigned char TxData)
{
    if (USART_handle->ISR & 0x80) // TXE
    {
        USART_handle->TDR = TxData;
        return 0;
    }
    return 1;
}

#define UART_TIMEOUT_ARRAY 0.0002
// @note 0.2ms timeout by default; optional DMA use
static inline void UART_SendArray(USART_TypeDef *USART_handle, unsigned char TxData[], unsigned char len, DMA_TypeDef *DMA_handle, void *DMA_handle_sub, unsigned char DMA_sub_ID)
{
    if (DMA_handle && DMA_handle_sub &&
        USART_handle->CR3 & 0x40) // DMAT
    {
#ifdef STM32H7
        // clear flags
        if (DMA_sub_ID >= 4)
            ((DMA_TypeDef *)DMA_handle)->HIFCR |= 0x3D << 6 * (DMA_sub_ID - 4);
        else
            ((DMA_TypeDef *)DMA_handle)->LIFCR |= 0x3D << 6 * DMA_sub_ID;

        ((DMA_Stream_TypeDef *)DMA_handle_sub)->NDTR = len;
        ((DMA_Stream_TypeDef *)DMA_handle_sub)->PAR = (unsigned)&USART_handle->TDR;
        ((DMA_Stream_TypeDef *)DMA_handle_sub)->M0AR = (unsigned)TxData;
        ((DMA_Stream_TypeDef *)DMA_handle_sub)->CR = 0x00010441;
#else
#error lack DMA config
#endif
    }
    else
    {
        timer_t USART_time = timer_InitStruct;
        unsigned char cnt = 0;
        while (!Timer_CheckTimeout(&USART_time, UART_TIMEOUT_ARRAY) && cnt < len)
            if (!UART_SendData(USART_handle, TxData[cnt]))
                cnt++;
    }
}
#undef UART_TIMEOUT_ARRAY

#define MODBUS_R_COIL 0x01
#define MODBUS_R_DISCRETE_INPUT 0x02
#define MODBUS_R_HOLDING_REG 0x03
#define MODBUS_R_INPUT_REG 0x04
#define MODBUS_W_SGL_COIL 0x05
#define MODBUS_W_SGL_REG 0x06
#define MODBUS_W_MPL_COIL 0x0F
#define MODBUS_W_MPL_REG 0x10

static inline void Modbus_UART_Read(USART_TypeDef *USART_handle, unsigned char slave_addr, unsigned char MODBUS_R_TYPE, unsigned short addr, unsigned short num, void *DMA_handle, void *DMA_handle_sub, unsigned char DMA_sub_ID)
{
    static unsigned char TxData[8];

    TxData[0] = slave_addr;
    TxData[1] = MODBUS_R_TYPE;
    TxData[2] = addr << 8;
    TxData[3] = addr;
    TxData[4] = num << 8;
    TxData[5] = num;
    *(unsigned short *)&TxData[6] = CRC_16_Cal(&CRC_16_Modbus, TxData, 6);

    UART_SendArray(USART_handle, TxData, 8, DMA_handle, DMA_handle_sub, DMA_sub_ID);
}

static inline void Modbus_UART_WriteSgl(USART_TypeDef *USART_handle, unsigned char slave_addr, unsigned char MODBUS_W_TYPE, unsigned short addr, unsigned short val, void *DMA_handle, void *DMA_handle_sub, unsigned char DMA_sub_ID)
{
    static unsigned char TxData[8];

    TxData[0] = slave_addr;
    TxData[1] = MODBUS_W_TYPE;
    TxData[2] = addr << 8;
    TxData[3] = addr;
    if (MODBUS_W_TYPE == MODBUS_W_SGL_COIL)
        *(unsigned short *)&TxData[4] = val ? 0xFF : 0;
    else if (MODBUS_W_TYPE == MODBUS_W_SGL_REG)
    {
        TxData[4] = val << 8;
        TxData[5] = val;
    }
    *(unsigned short *)&TxData[6] = CRC_16_Cal(&CRC_16_Modbus, TxData, 6);

    UART_SendArray(USART_handle, TxData, 8, DMA_handle, DMA_handle_sub, DMA_sub_ID);
}

// void USART3_IRQHandler(void)
// {
//     static unsigned char RxData[17], cnt;
//     // new msg
//     if (USART3->ISR & 0x20)
//     {
//         RxData[cnt++] = USART3->RDR;
//     }
//     // idle
//     else if (USART3->ISR & 0x10)
//     {
//         USART3->ICR |= 0x10;
//         cnt = 0;
//     }
//     // overrun
//     else if (USART3->ISR & 0x80)
//     {
//         USART3->ICR |= 0x80;
//     }
// }

#endif