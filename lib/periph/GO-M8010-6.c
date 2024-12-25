// unverified lib

#include "GO-M8010-6.h"
#include "USART.h"
#include "algorithm.h"

GO_M8010_6_t GO_M8010_6[GO_M8010_6_NUM];
unsigned char GO_M8010_6_TxData[17], GO_M8010_6_RxData[16];

void GO_M8010_6_SendCmd(void *RS485_handler, unsigned char ID)
{
    uint8_t ID_array = ID - GO_M8010_6_ID_OFFSET;

    *(unsigned short *)GO_M8010_6_TxData = GO_M8010_6_HEAD_SEND;
    GO_M8010_6_TxData[2] = ID | GO_M8010_6[ID_array].ctrl.mode << 4;

    LIMIT_ABS(GO_M8010_6[ID_array].ctrl.torque, GO_M8010_6_TORQUE_LIMIT);
    *(short *)&GO_M8010_6_TxData[3] = GO_M8010_6[ID_array].ctrl.torque * GO_M8010_6_fTORQUE;

    LIMIT_ABS(GO_M8010_6[ID_array].ctrl.spd, GO_M8010_6_SPD_LIMIT);
    *(short *)&GO_M8010_6_TxData[5] = GO_M8010_6[ID_array].ctrl.spd * GO_M8010_6_fSPD * GO_M8010_6_GR;

    LIMIT_ABS(GO_M8010_6[ID_array].ctrl.pos, GO_M8010_6_POS_LIMIT);
    *(int *)&GO_M8010_6_TxData[7] = GO_M8010_6[ID_array].ctrl.pos * GO_M8010_6_fPOS * GO_M8010_6_GR;

    LIMIT(GO_M8010_6[ID_array].ctrl.Kpos, GO_M8010_6_Kpos_LIMIT);
    *(unsigned short *)&GO_M8010_6_TxData[11] = GO_M8010_6[ID_array].ctrl.Kpos * GO_M8010_6_fKpos;

    LIMIT(GO_M8010_6[ID_array].ctrl.Kspd, GO_M8010_6_Kspd_LIMIT);
    *(unsigned short *)&GO_M8010_6_TxData[13] = GO_M8010_6[ID_array].ctrl.Kspd * GO_M8010_6_fKspd;

    *(unsigned short *)&GO_M8010_6_TxData[15] = crc_ccitt(0, GO_M8010_6_TxData, 15);

    UART_SendData(RS485_handler, GO_M8010_6_TxData, 17);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        if (*(unsigned short *)GO_M8010_6_RxData == GO_M8010_6_HEAD_RECV &&
            !crc_ccitt(0, GO_M8010_6_RxData, 16))
        {
            uint8_t ID_array = (GO_M8010_6_RxData[2] & 0x0F) - GO_M8010_6_ID_OFFSET;

            GO_M8010_6[ID_array].fdbk.mode = (GO_M8010_6_RxData[2] & 0x70) >> 4;
            GO_M8010_6[ID_array].fdbk.torque = *(short *)&GO_M8010_6_RxData[3] / GO_M8010_6_fTORQUE;
            GO_M8010_6[ID_array].fdbk.spd = *(short *)&GO_M8010_6_RxData[5] / GO_M8010_6_fSPD;
            GO_M8010_6[ID_array].fdbk.pos = *(int *)&GO_M8010_6_RxData[7] / GO_M8010_6_fPOS;
            GO_M8010_6[ID_array].fdbk.temp = GO_M8010_6_RxData[11];
            GO_M8010_6[ID_array].fdbk.err_motor = GO_M8010_6_RxData[12] & 0x7;
            GO_M8010_6[ID_array].fdbk.force = (*(unsigned short *)&GO_M8010_6_RxData[12] & 0xEFF8) >> 3;
        }
    }
}