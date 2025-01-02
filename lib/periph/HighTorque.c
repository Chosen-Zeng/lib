#include "HighTorque.h"
#include "HighTorque_REG.h"
#include "algorithm.h"

#include <string.h>

#if defined HIGHTORQUE_ID_OFFSET && defined HIGHTORQUE_NUM

/*hightorque_motor_t HTDW_4538_32_NE = {
                       //.GR = 32,
                       .torque_limit = 18,
                       .spd_limit = 100,
                       //.trq_k_f = 0.414104,
                       //.trq_b_f = -0.472467
},
                   HTDW_5047_36_NE = {.GR = 36,
                                      .torque_limit = 20,
                                      .spd_limit = 90};*/

hightorque_t HighTorque[HIGHTORQUE_NUM];

void HighTorque_SendPosParam_f(void *FDCAN_handle, unsigned char ID)
{
    unsigned char ID_array = ID - HIGHTORQUE_ID_OFFSET;
    unsigned char TxData[32] = {HIGHTORQUE_DATA_W | HIGHTORQUE_DATA_TYPE_8 | 1,
                                HIGHTORQUE_REG_MODE,
                                HIGHTORQUE_MODE_POS,
                                HIGHTORQUE_DATA_W | HIGHTORQUE_DATA_TYPE_FLOAT | HIGHTORQUE_MODE2,
                                5,
                                HIGHTORQUE_REG_POS_CTRL};

    *(float *)&TxData[6] = HighTorque[ID_array].ctrl.pos / 360;

    *(float *)&TxData[10] = HighTorque[ID_array].ctrl.spd / 360;

    // *(float *)&TxData[11] = (HighTorque[ID_array].ctrl.trq_offset - HTDW_motor->trq_b_f) / HTDW_motor->trq_k_f;
    *(float *)&TxData[14] = HighTorque[ID_array].ctrl.trq_offset;

    *(float *)&TxData[18] = HighTorque[ID_array].ctrl.Kp;
    *(float *)&TxData[22] = HighTorque[ID_array].ctrl.Kd;

    TxData[26] = HIGHTORQUE_DATA_R | HIGHTORQUE_DATA_TYPE_FLOAT | HIGHTORQUE_MODE2;
    TxData[27] = 3;
    TxData[28] = HIGHTORQUE_REG_POS_FDBK;

    memset(&TxData[29], HIGHTORQUE_NOP, 32 - 28 - 1);

    FDCAN_nBRS_SendData(FDCAN_handle, FDCAN_EXTENDED_ID, HIGHTORQUE_ADDR_RE | HIGHTORQUE_ADDR_SRC | ID, TxData, FDCAN_DLC_BYTES_32);
}

// @attention unverified feature
void HighTorque_Stop(void *FDCAN_handle, unsigned char ID)
{
    unsigned char TxData[5] = {HIGHTORQUE_DATA_W | HIGHTORQUE_DATA_TYPE_8 | 1,
                               HIGHTORQUE_REG_MODE,
                               HIGHTORQUE_MODE_STOP,
                               HIGHTORQUE_DATA_R | HIGHTORQUE_DATA_TYPE_8 | 1,
                               HIGHTORQUE_REG_MODE};

    FDCAN_nBRS_SendData(FDCAN_handle, FDCAN_EXTENDED_ID, HIGHTORQUE_ADDR_RE | HIGHTORQUE_ADDR_SRC | ID, TxData, FDCAN_DLC_BYTES_5);
}

// @attention unverified feature
void HighTorque_SetSpdLimit(void *FDCAN_handle, unsigned char ID, float spd, float acc)
{
    unsigned char TxData[12] = {HIGHTORQUE_DATA_W | HIGHTORQUE_DATA_TYPE_FLOAT | 2,
                                HIGHTORQUE_REG_SPD_LIMIT};

    *(float *)&TxData[2] = ABS(spd);
    *(float *)&TxData[6] = ABS(acc);

    TxData[10] = HIGHTORQUE_DATA_R | HIGHTORQUE_DATA_TYPE_FLOAT | 2;
    TxData[11] = HIGHTORQUE_REG_SPD_LIMIT;

    FDCAN_nBRS_SendData(FDCAN_handle, FDCAN_EXTENDED_ID, HIGHTORQUE_ADDR_RE | HIGHTORQUE_ADDR_SRC | ID, TxData, FDCAN_DLC_BYTES_12);
}

/*void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    uint8_t RxFifo0[32];
    FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxFifo0);

    switch ((unsigned long)hfdcan->Instance)
    {
    case (unsigned long)FDCAN3:
    {
        unsigned char ID_array = (FDCAN_RxHeader.Identifier >> 8) - HIGHTORQUE_ID_OFFSET;

        if (RxFifo0[0] == (HIGHTORQUE_DATA_RE | HIGHTORQUE_DATA_TYPE_FLOAT | HIGHTORQUE_MODE2) &&
            RxFifo0[1] == 3 &&
            RxFifo0[2] == HIGHTORQUE_REG_POS_FDBK)
        {
            HighTorque[ID_array].fdbk.pos = *(float *)&RxFifo0[3] * 360;
            HighTorque[ID_array].fdbk.spd = *(float *)&RxFifo0[7] * 360;
            // HighTorque[ID_array].fdbk.trq = *(float *)&RxFifo0[7] * HTDW_4538_32_NE.trq_k_f + HTDW_4538_32_NE.trq_b_f;
            HighTorque[ID_array].fdbk.trq = *(float *)&RxFifo0[7];
        }
        break;
    }
    }
}*/
#endif