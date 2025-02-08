#include "BRT.h"
#include "algorithm.h"
#include "CAN.h"

#if defined BRT_NUM && defined BRT_OFFSET

BRT_msg_t BRT_msg_ctrl;
float BRT_angle[BRT_NUM];

#define nDEBUG

#ifdef DEBUG
uint8_t BRT_status;
#endif

void BRT_SendCmd(void *CAN_handle, uint8_t ID, uint8_t BRT_func, uint32_t data)
{
    switch (BRT_func)
    {
    default:
    {
        BRT_msg_ctrl.length = 3 + 1;
        break;
    }
    case BRT_ATD_SET:
    case BRT_DPS_SMPLT_SET:
    {
        BRT_msg_ctrl.length = 3 + 2;
        break;
    }
    case BRT_POS_CURR_SET:
    {
        BRT_msg_ctrl.length = 3 + 4;
        break;
    }
    }
    BRT_msg_ctrl.addr = ID;
    BRT_msg_ctrl.func = BRT_func;
    *(uint32_t *)BRT_msg_ctrl.data = data;

#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_STD, BRT_msg_ctrl.addr, &BRT_msg_ctrl, BRT_msg_ctrl.length);
#elif defined FDCAN_SUPPROT
    FDCAN_SendData(CAN_handle, FDCAN_STANDARD_ID, BRT_msg_ctrl.addr, &BRT_msg_ctrl, BRT_msg_ctrl.length);
#endif
}

__weak void BRT_Init(void *CAN_handle, uint8_t ID)
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
    uint8_t RxFifo0[7];
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxFifo0);

    if (hfdcan->Instance == FDCAN1)
    {
        BRT_msg_t BRT_msg_fdbk;
        uint8_t count = FDCAN_RxHeader.Identifier - BRT_OFFSET;

        memcpy(&BRT_msg_fdbk, RxFifo0, RxFifo0[0]);

        switch (BRT_msg_fdbk.func)
        {
        case BRT_VAL_READ:
        {
            static float BRT_angle_curr[BRT_NUM], BRT_angle_prev[BRT_NUM];

#ifdef BRT_LAP_SGL
            BRT_angle_curr[count] = *(uint32_t *)BRT_msg_fdbk.data * BRT_fANGLE;

            if (ABS(BRT_angle_curr[count] - BRT_angle_prev[count]) >= 180)
                BRT_angle[count] += (BRT_angle_curr[count] > BRT_angle_prev[count] ? -360 : 360) + BRT_angle_curr[count] - BRT_angle_prev[count];
            else
                BRT_angle[count] += BRT_angle_curr[count] - BRT_angle_prev[count];
            BRT_angle_prev[count] = BRT_angle_curr[count];
#endif
#ifdef BRT_LAP_MPL
            BRT_angle_curr[count] = *(uint32_t *)BRT_msg_fdbk.data * BRT_fANGLE;

            if (ABS(BRT_angle_curr[count] - BRT_angle_prev[count]) >= BRT_LAP * 180)
                BRT_angle[count] += BRT_LAP * (BRT_angle_prev[count] > BRT_angle_curr[count] ? 360 : -360) + BRT_angle_curr[count] - BRT_angle_prev[count];
            else
                BRT_angle[count] += BRT_angle_curr[count] - BRT_angle_prev[count];
            BRT_angle_prev[count] = BRT_angle_curr[count];
#endif
            break;
        }
#ifdef DEBUG
        default:
        {
            BRT_status = BRT_msg_fdbk.data[0] ? BRT_ERROR : BRT_SUCCESS;

            break;
        }
#endif
        }
    }
}
#endif
#endif