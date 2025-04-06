#include "BRT.h"
#include "algorithm.h"
#include "CAN.h"

#if defined BRT_NUM && defined BRT_OFFSET

float BRT[BRT_NUM];

#define nDEBUG

#ifdef DEBUG
uint8_t BRT_status;
#endif

void BRT_SendCmd(void *CAN_handle, uint8_t ID, uint8_t BRT_func, uint32_t data)
{
    unsigned char TxData[7] = {3 + 1, ID, BRT_func};
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
    FDCAN_SendData(CAN_handle, FDCAN_STANDARD_ID, TxData[1], TxData, TxData[0]);
#endif
}

void BRT_Init(void *CAN_handle, uint8_t ID)
{
    BRT_SendCmd(CAN_handle, ID, BRT_ATD_SET, 1000);
    osDelay(500);
    BRT_SendCmd(CAN_handle, ID, BRT_MODE_SET, BRT_MODE_VAL);
    osDelay(500);
    BRT_SendCmd(CAN_handle, ID, BRT_INC_DIRCT_SET, BRT_INC_DIRCT_CW);
    osDelay(500);
    BRT_SendCmd(CAN_handle, ID, BRT_POS_0_SET, 0);
    osDelay(500);
}

#ifdef CAN_SUPPORT
#elif defined FDCAN_SUPPORT
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
    uint8_t RxData[7];
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxData);

    if (hfdcan->Instance == FDCAN1)
    {
        unsigned char arrID = FDCAN_RxHeader.Identifier - BRT_ID_OFFSET;

        switch (RxData[2])
        {
        case BRT_VAL_READ:
        {
            static float BRT_angle_curr[BRT_NUM], BRT_angle_prev[BRT_NUM];

#ifdef BRT_LAP_SGL
            BRT_angle_curr[arrID] = *(unsigned *)&TxData[3] * BRT_fANGLE;

            if (ABS(BRT_angle_curr[arrID] - BRT_angle_prev[arrID]) >= 180)
                BRT_angle[arrID] += (BRT_angle_curr[arrID] > BRT_angle_prev[arrID] ? -360 : 360) + BRT_angle_curr[arrID] - BRT_angle_prev[arrID];
            else
                BRT_angle[arrID] += BRT_angle_curr[arrID] - BRT_angle_prev[arrID];
            BRT_angle_prev[arrID] = BRT_angle_curr[arrID];
#endif
#ifdef BRT_LAP_MPL
            BRT_angle_curr[arrID] = *(unsigned *)&TxData[3] * BRT_fANGLE;

            if (ABS(BRT_angle_curr[arrID] - BRT_angle_prev[arrID]) >= BRT_LAP * 180)
                BRT_angle[arrID] += BRT_LAP * (BRT_angle_prev[arrID] > BRT_angle_curr[arrID] ? 360 : -360) + BRT_angle_curr[arrID] - BRT_angle_prev[arrID];
            else
                BRT_angle[arrID] += BRT_angle_curr[arrID] - BRT_angle_prev[arrID];
            BRT_angle_prev[arrID] = BRT_angle_curr[arrID];
#endif
            break;
        }
#ifdef DEBUG
        default:
        {
            BRT_status = TxData[3] ? BRT_ERROR : BRT_SUCCESS;

            break;
        }
#endif
        }
    }
}
#endif
#endif