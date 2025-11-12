#include "DaRan.h"

#ifdef DARAN_NUM

void DaRan_Init(CAN_handle_t *const CAN_handle, const unsigned char idx) {
    DaRan_Prop_W(CAN_handle, idx, DARAN_PARAM_CRASH_DETECT_EN, DARAN_DATA_TYPE_u32, 0);
}

// @note additional param needed in specific mode
void DaRan_SetPos(CAN_handle_t *const CAN_handle, const unsigned char idx, const unsigned char DARAN_CMD_POS_MODE, float param) {
    unsigned char TxData[8];

    *(float *)TxData = DaRan[idx].ctrl.pos;
    *(short *)&TxData[4] = LIMIT_RANGE(DaRan[idx].ctrl.spd, 32767 / DARAN_fSPD, -32768 / DARAN_fSPD) * DARAN_fSPD;

    switch (DARAN_CMD_POS_MODE) {
    case DARAN_CMD_POS_MODE_LIMIT:
    case DARAN_CMD_POS_MODE_FDFWD: {
        *(short *)&TxData[6] = LIMIT_RANGE(DaRan[idx].ctrl.trq, 32767 * DARAN_SCALING, -32768 * DARAN_SCALING) / DARAN_SCALING;
        break;
    }
    case DARAN_CMD_POS_MODE_FLTR: {
        *(short *)&TxData[6] = LIMIT(param, DARAN_FLTR_BW_MAX) / DARAN_SCALING;
        break;
    }
    case DARAN_CMD_POS_MODE_ACCEL: {
        *(short *)&TxData[6] = LIMIT_RANGE(param, 32767 * DARAN_SCALING, -32768 * DARAN_SCALING) / DARAN_SCALING;
        break;
    }
    }

#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_STD, DaRan[idx].ID << 5 | DARAN_CMD_POS_MODE, TxData, 8);
#elif defined FDCAN_SUPPORT
    CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, DaRan[idx].ID << 5 | DARAN_CMD_POS_MODE, TxData, 8);
#endif
}

void DaRan_SetSpd(CAN_handle_t *const CAN_handle, const unsigned char idx, const unsigned char DARAN_SPD_MODE, float accel) {
    float temp;

    unsigned char TxData[8];

    *(float *)TxData = temp = DaRan[idx].ctrl.spd / 60;
    *(short *)&TxData[4] = DARAN_SPD_MODE == DARAN_SPD_MODE_FDFWD ? LIMIT_RANGE(DaRan[idx].ctrl.trq, 32767 * DARAN_SCALING, -32768 * DARAN_SCALING) / DARAN_SCALING : LIMIT_RANGE(accel, 32767 * DARAN_SCALING, -32768 * DARAN_SCALING) / DARAN_SCALING;
    *(unsigned short *)&TxData[6] = DARAN_SPD_MODE;

#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_STD, DaRan[idx].ID << 5 | DARAN_CMD_SPD, TxData, 8);
#elif defined FDCAN_SUPPORT
    CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, DaRan[idx].ID << 5 | DARAN_CMD_SPD, TxData, 8);
#endif
}

void DaRan_SetTrq(CAN_handle_t *const CAN_handle, const unsigned char idx, const unsigned char DARAN_TRQ_MODE, float accel) {
    unsigned char TxData[8];

    *(float *)TxData = DaRan[idx].ctrl.trq;
    *(short *)&TxData[4] = DARAN_TRQ_MODE == DARAN_TRQ_MODE_DIRECT ? 0 : LIMIT_RANGE(accel, 32767 * DARAN_SCALING, -32768 * DARAN_SCALING) / DARAN_SCALING;
    *(unsigned short *)&TxData[6] = DARAN_TRQ_MODE;

#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_STD, DaRan[idx].ID << 5 | DARAN_CMD_TRQ, TxData, 8);
#elif defined FDCAN_SUPPORT
    CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, DaRan[idx].ID << 5 | DARAN_CMD_TRQ, TxData, 8);
#endif
}

void DaRan_Prop_W(CAN_handle_t *const CAN_handle, const unsigned char idx, const const unsigned short DARAN_PARAM, const unsigned short DARAN_DATA_TYPE, float param) {
    unsigned char TxData[8];

    *(unsigned short *)TxData = DARAN_PARAM;
    *(unsigned short *)&TxData[2] = DARAN_DATA_TYPE;
    switch (DARAN_DATA_TYPE) {
    case DARAN_DATA_TYPE_f: {
        *(float *)&TxData[4] = param;
        break;
    }
    case DARAN_DATA_TYPE_u16: {
        *(unsigned short *)&TxData[4] = LIMIT(param, 1 << 16 - 1);
        break;
    }
    case DARAN_DATA_TYPE_s16: {
        *(short *)&TxData[4] = LIMIT_RANGE(param, 1 << 15 - 1, -1 << 15);
        break;
    }
    case DARAN_DATA_TYPE_u32: {
        *(unsigned int *)&TxData[4] = LIMIT(param, 1 << 32 - 1);
        break;
    }
    case DARAN_DATA_TYPE_s32: {
        *(int *)&TxData[4] = LIMIT_RANGE(param, 1 << 31 - 1, -1 << 31);
        break;
    }
    }

#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_STD, DaRan[idx].ID << 5 | DARAN_CMD_PROP_W, TxData, 8);
#elif defined FDCAN_SUPPORT
    CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, DaRan[idx].ID << 5 | DARAN_CMD_PROP_W, TxData, 8);
#endif
}

void DaRan_Prop_R(CAN_handle_t *const const CAN_handle, const const unsigned char idx, const const unsigned short DARAN_PARAM, const unsigned short DARAN_DATA_TYPE) {
    unsigned char TxData[8];

    *(unsigned short *)TxData = DARAN_PARAM;
    *(unsigned short *)&TxData[2] = DARAN_DATA_TYPE;
    *(unsigned int *)&TxData[4] = 0;

#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_STD, DaRan[idx].ID << 5 | DARAN_CMD_PROP_W, TxData, 8);
#elif defined FDCAN_SUPPORT
    CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, DaRan[idx].ID << 5 | DARAN_CMD_PROP_W, TxData, 8);
#endif
}

bool DaRan_MsgHandler(const unsigned CAN_ID, const unsigned char idx, const unsigned char RxData[8])
{
    if (CAN_ID == (DaRan[idx].ID << 5 | DARAN_CMD_PROP_R))
    {
        DaRan[idx].fdbk.pos = *(float *)RxData;
        DaRan[idx].fdbk.spd = *(short *)&RxData[4] / DARAN_fSPD;
        DaRan[idx].fdbk.trq = *(short *)&RxData[6] * DARAN_SCALING;

        return false;
    }
    return true;
}

#else
#error DARAN_NUM undefined
#endif