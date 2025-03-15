#include "GO-M8010-6.h"
#include "USART.h"
#include "algorithm.h"
#include "CRC.h"

#if defined GO_M8010_6_NUM && defined GO_M8010_6_ID_OFFSET

GO_M8010_6_t GO_M8010_6[GO_M8010_6_NUM];
GO_M8010_6_data_t GO_M8010_6_data[DMA_Stream_PAIR]; // for DMA

void GO_M8010_6_SendCmd(USART_info_t *UART_info, unsigned char ID)
{
    unsigned char ID_array = ID - GO_M8010_6_ID_OFFSET;

    *(unsigned short *)GO_M8010_6_data[ID_array].TxData = GO_M8010_6_HEAD_SEND;
    GO_M8010_6_data[ID_array].TxData[2] = ID | GO_M8010_6[ID_array].ctrl.mode << 4;
    *(short *)&GO_M8010_6_data[ID_array].TxData[3] = LIMIT_ABS(GO_M8010_6[ID_array].ctrl.torque, GO_M8010_6_TORQUE_LIMIT) * GO_M8010_6_fTORQUE;
    *(short *)&GO_M8010_6_data[ID_array].TxData[5] = LIMIT_ABS(GO_M8010_6[ID_array].ctrl.spd, GO_M8010_6_SPD_LIMIT) * GO_M8010_6_fSPD;
    *(int *)&GO_M8010_6_data[ID_array].TxData[7] = LIMIT_ABS(GO_M8010_6[ID_array].ctrl.pos, GO_M8010_6_POS_LIMIT) * GO_M8010_6_fPOS;
    *(unsigned short *)&GO_M8010_6_data[ID_array].TxData[11] = LIMIT(GO_M8010_6[ID_array].ctrl.Kpos, GO_M8010_6_Kpos_LIMIT) * GO_M8010_6_fKpos;
    *(unsigned short *)&GO_M8010_6_data[ID_array].TxData[13] = LIMIT(GO_M8010_6[ID_array].ctrl.Kspd, GO_M8010_6_Kspd_LIMIT) * GO_M8010_6_fKspd;
    *(unsigned short *)&GO_M8010_6_data[ID_array].TxData[15] = CRC_16_Cal(&CRC_16_CCITT, GO_M8010_6_data[ID_array].TxData, 15);

    UART_SendArray(UART_info, GO_M8010_6_data[ID_array].TxData, 17);
}

// void USART3_IRQHandler(void)
// {
//     static unsigned char RxData[16], cnt;
//     if (USART3->ISR & 0x20) // new message
//     {
//         RxData[cnt++] = USART3->RDR;
//     }
//     else if (USART3->ISR & 0x10) // idle
//     {
//         USART3->ICR |= 0x10;
//         cnt = 0;

//         if (*(unsigned short *)RxData == GO_M8010_6_HEAD_RECV &&
//             !CRC_16_Cal(&CRC_16_CCITT, RxData, 16))
//         {
//             uint8_t ID_array = (RxData[2] & 0x0F) - GO_M8010_6_ID_OFFSET;

//             GO_M8010_6[ID_array].fdbk.mode = (RxData[2] & 0x70) >> 4;
//             GO_M8010_6[ID_array].fdbk.torque = *(short *)&RxData[3] / GO_M8010_6_fTORQUE;
//             GO_M8010_6[ID_array].fdbk.spd = *(short *)&RxData[5] / GO_M8010_6_fSPD;
//             GO_M8010_6[ID_array].fdbk.pos = *(int *)&RxData[7] / GO_M8010_6_fPOS;
//             GO_M8010_6[ID_array].fdbk.temp = RxData[11];
//             GO_M8010_6[ID_array].fdbk.err_motor = RxData[12] & 0x7;
//             GO_M8010_6[ID_array].fdbk.force = (*(unsigned short *)&RxData[12] & 0xEFF8) >> 3;
//         }
//     }
//     else if (USART3->ISR & 0x80) // overrun
//     {
//         USART3->ICR |= 0x80;
//     }
// }

// void DMA1_Stream2_IRQHandler(void)
// {
//     if (DMA1->LISR & 0x200000)
//     {
//         DMA1->LIFCR |= 0x200000;

//         if (*(unsigned short *)GO_M8010_6_data[0].RxData == GO_M8010_6_HEAD_RECV &&
//             !CRC_16_Cal(&CRC_16_CCITT, GO_M8010_6_data[0].RxData, 16))
//         {
//             uint8_t ID_array = (GO_M8010_6_data[0].RxData[2] & 0x0F) - GO_M8010_6_ID_OFFSET;

//             GO_M8010_6[ID_array].fdbk.mode = (GO_M8010_6_data[0].RxData[2] & 0x70) >> 4;
//             GO_M8010_6[ID_array].fdbk.torque = *(short *)&GO_M8010_6_data[0].RxData[3] / GO_M8010_6_fTORQUE;
//             GO_M8010_6[ID_array].fdbk.spd = *(short *)&GO_M8010_6_data[0].RxData[5] / GO_M8010_6_fSPD;
//             GO_M8010_6[ID_array].fdbk.pos = *(int *)&GO_M8010_6_data[0].RxData[7] / GO_M8010_6_fPOS;
//             GO_M8010_6[ID_array].fdbk.temp = GO_M8010_6_data[0].RxData[11];
//             GO_M8010_6[ID_array].fdbk.err_motor = GO_M8010_6_data[0].RxData[12] & 0x7;
//             GO_M8010_6[ID_array].fdbk.force = (*(unsigned short *)&GO_M8010_6_data[0].RxData[12] & 0xEFF8) >> 3;
//         }
//     }
// }
#endif