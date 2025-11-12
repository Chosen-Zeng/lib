#ifndef __USART_H
#define __USART_H

#include "CRC.h"
#include "TIM.h"

typedef const struct {
    USART_TypeDef *USART_handle;
    float timeout;
    DMA_TypeDef *DMA_handle;
    void *DMA_subhandle;
    unsigned char DMA_ID;
} USART_handle_t;

static inline unsigned char UART_SendData(USART_handle_t *const USART_handle, const unsigned char TxData) {
#if defined STM32H7
    if (USART_handle->USART_handle->ISR & 0x80) // TXE
    {
        USART_handle->USART_handle->TDR = TxData;
#elif (defined STM32F4 || \
       defined STM32F1)
    if (USART_handle->USART_handle->SR & 0x80) // TXE
    {
        USART_handle->USART_handle->DR = TxData;
#else
#error No UART cfg.
#endif
        return 0;
    }
    return 1;
}

#define UART_TIMEOUT_ARRAY 0.0005
// @note 0.5ms timeout by default
static inline void UART_SendArray(USART_handle_t *const USART_handle, const unsigned char TxData[], const unsigned char len) {

#if (defined STM32H7 || \
     defined STM32F4)
    if (USART_handle->DMA_handle && USART_handle->DMA_subhandle) // DMA cfg
    {
        // clear flags
        if (USART_handle->DMA_ID > 5)
            USART_handle->DMA_handle->HIFCR |= 0x20 << (6 * (USART_handle->DMA_ID - 4) + 4);
        else if (USART_handle->DMA_ID > 3)
            USART_handle->DMA_handle->HIFCR |= 0x20 << 6 * (USART_handle->DMA_ID - 4);
        else if (USART_handle->DMA_ID > 1)
            USART_handle->DMA_handle->LIFCR |= 0x20 << (6 * USART_handle->DMA_ID + 4);
        else
            USART_handle->DMA_handle->LIFCR |= 0x20 << 6 * USART_handle->DMA_ID;

        ((DMA_Stream_TypeDef *)USART_handle->DMA_subhandle)->NDTR = len;
        ((DMA_Stream_TypeDef *)USART_handle->DMA_subhandle)->PAR = (unsigned)&USART_handle->USART_handle->TDR;
        ((DMA_Stream_TypeDef *)USART_handle->DMA_subhandle)->M0AR = (unsigned)TxData;
        USART_handle->USART_handle->CR3 |= 0x80;
        ((DMA_Stream_TypeDef *)USART_handle->DMA_subhandle)->CR |= 1;
    } else
#elif defined STM32F1
    if (USART_handle->DMA_handle && USART_handle->DMA_subhandle) // DMA cfg
    {
        // clear flags
        USART_handle->DMA_handle->IFCR |= 0x1 << 4 * (USART_handle->DMA_ID - 1);

        ((DMA_Channel_TypeDef *)USART_handle->DMA_subhandle)->CCR &= ~1;
        ((DMA_Channel_TypeDef *)USART_handle->DMA_subhandle)->CNDTR = len;
        ((DMA_Channel_TypeDef *)USART_handle->DMA_subhandle)->CPAR = (unsigned)&USART_handle->USART_handle->DR;
        ((DMA_Channel_TypeDef *)USART_handle->DMA_subhandle)->CMAR = (unsigned)TxData;
        USART_handle->USART_handle->CR3 |= 0x80;
        ((DMA_Channel_TypeDef *)USART_handle->DMA_subhandle)->CCR |= 1;
    } else
#else
#warning No DMA cfg.
#endif
    {
#ifdef TIMsw
        TIMsw_t USART_time = TIMsw_InitStruct;
        unsigned short cnt = 0;
        while (!TIMsw_CheckTimeout(&USART_time, USART_handle->timeout ? USART_handle->timeout : UART_TIMEOUT_ARRAY) && cnt < len)
            if (!UART_SendData(USART_handle, TxData[cnt]))
                cnt++;
#else
#warning UART may block the program.
        unsigned short cnt = 0;
        while (cnt < len)
            if (!UART_SendData(USART_handle, TxData[cnt]))
                cnt++;
#endif
    }
}

#define MODBUS_R_COIL           0x01
#define MODBUS_R_DISCRETE_INPUT 0x02
#define MODBUS_R_HOLDING_REG    0x03
#define MODBUS_R_INPUT_REG      0x04
#define MODBUS_W_SGL_COIL       0x05
#define MODBUS_W_SGL_REG        0x06
#define MODBUS_W_MPL_COIL       0x0F
#define MODBUS_W_MPL_REG        0x10

static inline void Modbus_UART_Read(USART_handle_t *const USART_handle, const unsigned char slave_addr, const unsigned char MODBUS_R_TYPE, const unsigned short addr, const unsigned short num) {
    unsigned char TxData[8] = {slave_addr,
                               MODBUS_R_TYPE,
                               addr << 8,
                               addr,
                               num << 8,
                               num};
    *(unsigned short *)&TxData[6] = CRCsw_Calc(&CRC_16_MODBUS, TxData, 6);

    UART_SendArray(USART_handle, TxData, 8);
}

static inline void Modbus_UART_WriteSgl(USART_handle_t *const USART_handle, const unsigned char slave_addr, const unsigned char MODBUS_W_TYPE, const unsigned short addr, const unsigned short val) {
    unsigned char TxData[8] = {slave_addr,
                               MODBUS_W_TYPE,
                               addr << 8,
                               addr};

    if (MODBUS_W_TYPE == MODBUS_W_SGL_COIL)
        *(unsigned short *)&TxData[4] = val ? 0xFF : 0;
    else if (MODBUS_W_TYPE == MODBUS_W_SGL_REG) {
        TxData[4] = val << 8;
        TxData[5] = val;
    }
    *(unsigned short *)&TxData[6] = CRCsw_Calc(&CRC_16_MODBUS, TxData, 6);

    UART_SendArray(USART_handle, TxData, 8);
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
//     else if (USART3->ISR & 0x8)
//     {
//         USART3->ICR |= 0x8;
//     }
// }

#endif