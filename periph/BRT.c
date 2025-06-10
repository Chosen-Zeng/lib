#include "BRT.h"
#include "CAN.h"

#if defined BRT_NUM

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
    CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, TxData[1], TxData, TxData[0]);
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

void FDCAN1_IT0_IRQHandler(void)
{
    if (FDCAN1->IR & 0x1)
    {
        FDCAN1->IR |= 0x1;

        FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
        unsigned char RxData[32];
        HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxData);

        switch (FDCAN_RxHeader.Identifier)
        {
        case BRT_ID:
        {
            switch (RxData[2])
            {
            case BRT_VAL_READ:
            {
                static float BRT_curr[BRT_NUM], BRT_prev[BRT_NUM];

#ifdef BRT_LAP_SGL
                BRT_curr[BRT_arrID] = *(unsigned *)&TxData[3] * BRT_fANGLE;

                if (ABS(BRT_curr[BRT_arrID] - BRT_prev[BRT_arrID]) >= 180)
                    BRT[BRT_arrID] += (BRT_curr[BRT_arrID] > BRT_prev[BRT_arrID] ? -360 : 360) + BRT_curr[BRT_arrID] - BRT_prev[BRT_arrID];
                else
                    BRT[BRT_arrID] += BRT_curr[BRT_arrID] - BRT_prev[BRT_arrID];
                BRT_prev[BRT_arrID] = BRT_curr[BRT_arrID];
#endif
#ifdef BRT_LAP_MPL
                BRT_curr[BRT_arrID] = *(unsigned *)&TxData[3] * BRT_fANGLE;

                if (ABS(BRT_curr[BRT_arrID] - BRT_prev[BRT_arrID]) >= BRT_LAP * 180)
                    BRT[BRT_arrID] += BRT_LAP * (BRT_prev[BRT_arrID] > BRT_curr[BRT_arrID] ? 360 : -360) + BRT_curr[BRT_arrID] - BRT_prev[BRT_arrID];
                else
                    BRT[BRT_arrID] += BRT_curr[BRT_arrID] - BRT_prev[BRT_arrID];
                BRT_prev[BRT_arrID] = BRT_curr[BRT_arrID];
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
    }
}
#endif