#include "BRT.h"
#include "CAN.h"

#ifdef BRT_NUM

void BRT_SendCmd(CAN_handle_t *const CAN_handle, const unsigned char arrID, const unsigned char BRT_func, const unsigned data)
{
    unsigned char TxData[7] = {3 + 1, BRT[arrID].ID, BRT_func};
    switch (BRT_func)
    {
    case BRT_ATD_SET:
    case BRT_DPS_SMPLT_SET:
    {
        TxData[0] = 3 + 2;
        break;
    }
    case BRT_POS_CURR_SET:
    {
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

void BRT_Init(CAN_handle_t *const CAN_handle, const unsigned char arrID)
{
    BRT_SendCmd(CAN_handle, arrID, BRT_ATD_SET, 1000);
    BRT_SendCmd(CAN_handle, arrID, BRT_MODE_SET, BRT_MODE_VAL);
    BRT_SendCmd(CAN_handle, arrID, BRT_INC_DIRCT_SET, BRT_INC_DIRCT_CW);
    BRT_SendCmd(CAN_handle, arrID, BRT_POS_0_SET, 0);
}

bool BRT_MsgHandler(const unsigned CAN_ID, const unsigned char arrID, const unsigned char RxData[7])
{
    if (CAN_ID == BRT[arrID].ID &&
        BRT[arrID].ID == RxData[1] &&
        BRT_VAL_READ == RxData[2])
    {
        static float BRT_curr[BRT_NUM], BRT_prev[BRT_NUM];

        BRT_curr[arrID] = *(unsigned *)&TxData[3] * 360.f / BRT_handle[arrID].res;

        if (ABS(BRT_curr[arrID] - BRT_prev[arrID]) >= 180 * BRT_handle[arrID].lap)
            BRT[arrID] += (BRT_curr[arrID] > BRT_prev[arrID] ? -360 : 360) * BRT_handle[arrID].lap + BRT_curr[arrID] - BRT_prev[arrID];
        else
            BRT[arrID] += BRT_curr[arrID] - BRT_prev[arrID];

        BRT_prev[arrID] = BRT_curr[arrID];

        return true;
    }
    return false;
}

#else
#error BRT_NUM undefined
#endif