#include "BRT.h"
#include "CAN.h"

#ifdef BRT_NUM

void BRT_SendCmd(CAN_handle_t *const CAN_handle, const unsigned char idx, const unsigned char BRT_func, const unsigned data) {
    unsigned char TxData[7] = {3 + 1, BRT[idx].ID, BRT_func};
    switch (BRT_func) {
    case BRT_ATD_SET:
    case BRT_DPS_SMPLT_SET: {
        TxData[0] = 3 + 2;
        break;
    }
    case BRT_POS_CURR_SET: {
        TxData[0] = 3 + 4;
        break;
    }
    }
    *(unsigned *)&TxData[3] = data;

#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_STD, TxData[1], TxData, TxData[0]);
#elif defined FDCAN_SUPPROT
    CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, TxData[1], TxData, TxData[0]);
#endif
}

void BRT_Init(CAN_handle_t *const CAN_handle, const unsigned char idx) {
    BRT_SendCmd(CAN_handle, idx, BRT_ATD_SET, 1000);
    BRT_SendCmd(CAN_handle, idx, BRT_MODE_SET, BRT_MODE_VAL);
    BRT_SendCmd(CAN_handle, idx, BRT_INC_DIRCT_SET, BRT_INC_DIRCT_CW);
    BRT_SendCmd(CAN_handle, idx, BRT_POS_0_SET, 0);
}

bool BRT_MsgHandler(const unsigned CAN_ID, const unsigned char idx, const unsigned char RxData[7]) {
    if (CAN_ID == BRT[idx].ID &&
        BRT[idx].ID == RxData[1] &&
        BRT_VAL_READ == RxData[2]) {
        static float BRT_curr[BRT_NUM], BRT_prev[BRT_NUM];

        BRT_curr[idx] = *(unsigned *)&TxData[3] * 360.f / BRT_handle[idx].res;

        if (ABS(BRT_curr[idx] - BRT_prev[idx]) >= 180 * BRT_handle[idx].lap)
            BRT[idx] += (BRT_curr[idx] > BRT_prev[idx] ? -360 : 360) * BRT_handle[idx].lap + BRT_curr[idx] - BRT_prev[idx];
        else
            BRT[idx] += BRT_curr[idx] - BRT_prev[idx];

        BRT_prev[idx] = BRT_curr[idx];

        return false;
    }
    return true;
}

#else
#error BRT_NUM undefined
#endif