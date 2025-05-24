/*
    source address: 0 (default)
*/

#include "HighTorque.h"
#include "CAN.h"
#include "algorithm.h"

#if defined HIGHTORQUE_ID_OFFSET && defined HIGHTORQUE_NUM

HighTorque_t HighTorque[HIGHTORQUE_NUM + 1];

void HighTorque_SetMixParam_f(void *FDCAN_handle, unsigned char ID)
{
    unsigned char arrID = ID == HIGHTORQUE_ADDR_BCAST ? HIGHTORQUE_NUM : ID - HIGHTORQUE_ID_OFFSET;
    unsigned char TxData[32] = {HIGHTORQUE_DATA_W | HIGHTORQUE_DATA_TYPE_8b | 1,
                                HIGHTORQUE_REG_MODE,
                                HIGHTORQUE_MODE_POS,
                                HIGHTORQUE_DATA_W | HIGHTORQUE_DATA_TYPE_FLOAT | HIGHTORQUE_DATA_MODE2,
                                5,
                                HIGHTORQUE_REG_POS_CTRL,
                                [26] = HIGHTORQUE_DATA_R | HIGHTORQUE_DATA_TYPE_FLOAT | 3,
                                HIGHTORQUE_REG_POS_FDBK,
                                HIGHTORQUE_DATA_R | HIGHTORQUE_DATA_TYPE_FLOAT | 1,
                                HIGHTORQUE_REG_TEMP,
                                HIGHTORQUE_NOP,
                                HIGHTORQUE_NOP};

    *(float *)&TxData[6] = HighTorque[arrID].ctrl.pos / 360;
    *(float *)&TxData[10] = HighTorque[arrID].ctrl.spd / 360;
    *(float *)&TxData[14] = HighTorque[arrID].ctrl.trq;
    *(float *)&TxData[18] = LIMIT(HighTorque[arrID].ctrl.Kp, HIGHTORQUE_NO_LIMIT_FLOAT);
    *(float *)&TxData[22] = LIMIT(HighTorque[arrID].ctrl.Kd, HIGHTORQUE_NO_LIMIT_FLOAT);

    FDCAN_nBRS_SendData(FDCAN_handle, FDCAN_EXTENDED_ID, HIGHTORQUE_ADDR_RE | ID, TxData, FDCAN_DLC_BYTES_32);
}

void HighTorque_SwitchMode(void *FDCAN_handle, unsigned char ID, unsigned char HIGHTORQUE_MODE)
{
    unsigned char TxData[5] = {HIGHTORQUE_DATA_W | HIGHTORQUE_DATA_TYPE_8b | 1,
                               HIGHTORQUE_REG_MODE,
                               HIGHTORQUE_MODE,
                               HIGHTORQUE_DATA_R | HIGHTORQUE_DATA_TYPE_8b | 1,
                               HIGHTORQUE_REG_MODE};

    FDCAN_nBRS_SendData(FDCAN_handle, FDCAN_EXTENDED_ID, HIGHTORQUE_ADDR_RE | ID, TxData, 5);
}

// ineffective currently
void HighTorque_SetSpdLimit(void *FDCAN_handle, unsigned char ID, float spd, float acc)
{
    unsigned char TxData[12] = {HIGHTORQUE_DATA_W | HIGHTORQUE_DATA_TYPE_FLOAT | 2,
                                HIGHTORQUE_REG_SPD_LIMIT,
                                [10] = HIGHTORQUE_DATA_R | HIGHTORQUE_DATA_TYPE_FLOAT | 2,
                                HIGHTORQUE_REG_SPD_LIMIT};

    *(float *)&TxData[2] = ABS(spd) / 360;
    *(float *)&TxData[6] = ABS(acc) / 360;

    FDCAN_nBRS_SendData(FDCAN_handle, FDCAN_EXTENDED_ID, HIGHTORQUE_ADDR_RE | ID, TxData, FDCAN_DLC_BYTES_12);
}

/* void FDCAN1_IT0_IRQHandler(void)
{
    if (FDCAN1->IR & 0x1)
    {
        FDCAN1->IR |= 0x1;

        FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
        uint8_t RxData[64];
        HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxData);

        unsigned char arrID = (FDCAN_RxHeader.Identifier >> 8) - HIGHTORQUE_ID_OFFSET;

        if (RxData[0] == (HIGHTORQUE_DATA_RE | HIGHTORQUE_DATA_TYPE_FLOAT | 3) &&
            RxData[1] == HIGHTORQUE_REG_POS_FDBK &&
            RxData[14] == (HIGHTORQUE_DATA_RE | HIGHTORQUE_DATA_TYPE_FLOAT | 1) &&
            RxData[15] == HIGHTORQUE_REG_TEMP)
        {
            HighTorque[2 - HIGHTORQUE_ID_OFFSET].fdbk.pos = *(float *)&RxData[2] * 360;
            HighTorque[2 - HIGHTORQUE_ID_OFFSET].fdbk.spd = *(float *)&RxData[6] * 360;
            HighTorque[2 - HIGHTORQUE_ID_OFFSET].fdbk.trq = *(float *)&RxData[10];
            // HighTorque[arrID].fdbk.trq = *(float *)&RxData[10] * HTDW_4538_32_NE.trq_k + HTDW_4538_32_NE.trq_d;
            HighTorque[2 - HIGHTORQUE_ID_OFFSET].fdbk.temp = *(float *)&RxData[16];
        }
    }
}
*/
#endif