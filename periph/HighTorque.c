#include "HighTorque.h"
#include "CAN.h"
#include "algorithm.h"

#ifdef FDCAN_SUPPORT
#ifdef HIGHTORQUE_NUM

void HighTorque_SetMixParam_f(CAN_handle_t *const CAN_handle, const unsigned char idx)
{
    float temp;

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

    *(float *)&TxData[6] = temp = HighTorque[idx].ctrl.pos / 360;
    *(float *)&TxData[10] = temp = HighTorque[idx].ctrl.spd / 360;
    *(float *)&TxData[14] = HighTorque[idx].ctrl.trq;
    *(float *)&TxData[18] = LIMIT(HighTorque[idx].ctrl.Kp, HIGHTORQUE_NO_LIMIT_FLOAT);
    *(float *)&TxData[22] = LIMIT(HighTorque[idx].ctrl.Kd, HIGHTORQUE_NO_LIMIT_FLOAT);

    FDCAN_nBRS_SendData(CAN_handle, FDCAN_EXTENDED_ID, HIGHTORQUE_ID_RE | HighTorque[idx].ID, TxData, FDCAN_DLC_BYTES_32);
}

void HighTorque_SwitchMode(CAN_handle_t *const CAN_handle, const unsigned char idx, const unsigned char HIGHTORQUE_MODE)
{
    unsigned char TxData[5] = {HIGHTORQUE_DATA_W | HIGHTORQUE_DATA_TYPE_8b | 1,
                               HIGHTORQUE_REG_MODE,
                               HIGHTORQUE_MODE,
                               HIGHTORQUE_DATA_R | HIGHTORQUE_DATA_TYPE_8b | 1,
                               HIGHTORQUE_REG_MODE};

    FDCAN_nBRS_SendData(CAN_handle, FDCAN_EXTENDED_ID, HIGHTORQUE_ID_RE | HighTorque[idx].ID, TxData, 5);
}

// ineffective currently
void HighTorque_SetSpdLimit(CAN_handle_t *const CAN_handle, const unsigned char idx, const float spd, const float acc)
{
    float temp;

    unsigned char TxData[12] = {HIGHTORQUE_DATA_W | HIGHTORQUE_DATA_TYPE_FLOAT | 2,
                                HIGHTORQUE_REG_SPD_LIMIT,
                                [10] = HIGHTORQUE_DATA_R | HIGHTORQUE_DATA_TYPE_FLOAT | 2,
                                HIGHTORQUE_REG_SPD_LIMIT};

    *(float *)&TxData[2] = temp = ABS(spd) / 360;
    *(float *)&TxData[6] = temp = ABS(acc) / 360;

    FDCAN_nBRS_SendData(CAN_handle, FDCAN_EXTENDED_ID, HIGHTORQUE_ID_RE | HighTorque[idx].ID, TxData, FDCAN_DLC_BYTES_12);
}

bool HighTorque_MsgHandler(const unsigned CAN_ID, const unsigned char idx, const unsigned char RxData[20])
{
    if (CAN_ID == (HighTorque[idx].ID << 8 | HIGHTORQUE_ID_SRC) &&
        RxData[0] == (HIGHTORQUE_DATA_RE | HIGHTORQUE_DATA_TYPE_FLOAT | 3) &&
        RxData[1] == HIGHTORQUE_REG_POS_FDBK &&
        RxData[14] == (HIGHTORQUE_DATA_RE | HIGHTORQUE_DATA_TYPE_FLOAT | 1) &&
        RxData[15] == HIGHTORQUE_REG_TEMP)
    {
        HighTorque[idx].fdbk.pos = *(float *)&RxData[2] * 360;
        HighTorque[idx].fdbk.spd = *(float *)&RxData[6] * 360;
        HighTorque[idx].fdbk.trq = *(float *)&RxData[10];
        HighTorque[idx].fdbk.temp = *(float *)&RxData[16];

        return true;
    }
    return false;
}

#else
#error HIGHTORQUE_NUM undefined
#endif
#else
#error FDCAN unsupported
#endif