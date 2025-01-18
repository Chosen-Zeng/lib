// unverified lib

#include "GO-M8010-6.h"
#include "USART.h"
#include "algorithm.h"
#include "CRC.h"

#if defined GO_M8010_6_NUM && defined GO_M8010_6_ID_OFFSET

GO_M8010_6_t GO_M8010_6[GO_M8010_6_NUM];

void GO_M8010_6_SendCmd(void *RS485_handler, unsigned char ID)
{
    unsigned char ID_array = ID - GO_M8010_6_ID_OFFSET, TxData[17];

    *(unsigned short *)TxData = GO_M8010_6_HEAD_SEND;
    TxData[2] = ID | GO_M8010_6[ID_array].ctrl.mode << 4;

    LIMIT_ABS(GO_M8010_6[ID_array].ctrl.torque, GO_M8010_6_TORQUE_LIMIT);
    *(short *)&TxData[3] = GO_M8010_6[ID_array].ctrl.torque * GO_M8010_6_fTORQUE;

    LIMIT_ABS(GO_M8010_6[ID_array].ctrl.spd, GO_M8010_6_SPD_LIMIT);
    *(short *)&TxData[5] = GO_M8010_6[ID_array].ctrl.spd * GO_M8010_6_fSPD * GO_M8010_6_GR;

    LIMIT_ABS(GO_M8010_6[ID_array].ctrl.pos, GO_M8010_6_POS_LIMIT);
    *(int *)&TxData[7] = GO_M8010_6[ID_array].ctrl.pos * GO_M8010_6_fPOS * GO_M8010_6_GR;

    LIMIT(GO_M8010_6[ID_array].ctrl.Kpos, GO_M8010_6_Kpos_LIMIT);
    *(unsigned short *)&TxData[11] = GO_M8010_6[ID_array].ctrl.Kpos * GO_M8010_6_fKpos;

    LIMIT(GO_M8010_6[ID_array].ctrl.Kspd, GO_M8010_6_Kspd_LIMIT);
    *(unsigned short *)&TxData[13] = GO_M8010_6[ID_array].ctrl.Kspd * GO_M8010_6_fKspd;

    *(unsigned short *)&TxData[15] = CRC_16_Cal(&CRC_16_CCITT, TxData, 15);

    UART_SendData(RS485_handler, TxData, 17);
}

__weak void USART3_IRQHandler(void)
{
    static unsigned char RxData[16], cnt;
    if (USART3->ISR & 0x20)
    {
        RxData[cnt++] = USART3->RDR;
    }
    else if (USART3->ISR & 0x10)
    {
        USART3->ICR |= 0x10;
        cnt = 0;

        if (*(unsigned short *)RxData == GO_M8010_6_HEAD_RECV &&
            !CRC_16_Cal(&CRC_16_CCITT, RxData, 16) &&
            (RxData[2] & 0xF) - GO_M8010_6_ID_OFFSET < GO_M8010_6_NUM &&
            (RxData[2] & 0xF) - GO_M8010_6_ID_OFFSET >= 0)
        {
            uint8_t ID_array = (RxData[2] & 0x0F) - GO_M8010_6_ID_OFFSET;

            GO_M8010_6[ID_array].fdbk.mode = (RxData[2] & 0x70) >> 4;
            GO_M8010_6[ID_array].fdbk.torque = *(short *)&RxData[3] / GO_M8010_6_fTORQUE;
            GO_M8010_6[ID_array].fdbk.spd = *(short *)&RxData[5] / GO_M8010_6_fSPD;
            GO_M8010_6[ID_array].fdbk.pos = *(int *)&RxData[7] / GO_M8010_6_fPOS;
            GO_M8010_6[ID_array].fdbk.temp = RxData[11];
            GO_M8010_6[ID_array].fdbk.err_motor = RxData[12] & 0x7;
            GO_M8010_6[ID_array].fdbk.force = (*(unsigned short *)&RxData[12] & 0xEFF8) >> 3;
        }
    }
}
#endif