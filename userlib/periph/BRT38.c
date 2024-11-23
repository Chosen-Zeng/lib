#include "cmsis_os.h"
#include "BRT38.h"
#include "BRT38_INST.h"
#ifdef CAN_SUPPORT
#include "CAN.h"
#elif defined FDCAN_SUPPORT
#include "FDCAN.h"
#endif

#define ABS(X) ((X) >= 0 ? (X) : -(X)) // 输出X绝对值

BRT38_msg_t BRT38_msg_ctrl;
float BRT38_angle;

#ifdef DEBUG
uint8_t BRT38_status;
#endif

static void BRT38_SendCmd(void *CAN_handle, uint8_t ID, uint8_t BRT38_func, uint32_t data)
{
    switch (BRT38_func)
    {
    default:
    {
        BRT38_msg_ctrl.length = 1 + 3;
        break;
    }
    case BRT38_ATD_SET:
    case BRT38_DPS_SMPLT_SET:
    {
        BRT38_msg_ctrl.length = 2 + 3;
        break;
    }
    case BRT38_POS_CURR_SET:
    {
        BRT38_msg_ctrl.length = 4 + 3;
        break;
    }
    }
    BRT38_msg_ctrl.addr = ID;
    BRT38_msg_ctrl.func = BRT38_func;
    *(uint32_t *)BRT38_msg_ctrl.data = data;

#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_STD, BRT38_msg_ctrl.addr, &BRT38_msg_ctrl, BRT38_msg_ctrl.length);
#elif defined FDCAN_SUPPROT
    FDCAN_SendData(CAN_handle, FDCAN_STANDARD_ID, BRT38_msg_ctrl.addr, &BRT38_msg_ctrl, BRT38_msg_ctrl.length);
#endif
}

void BRT38_Init(void *CAN_handle, uint8_t ID)
{
    BRT38_SendCmd(CAN_handle, ID, BRT38_ATD_SET, 1000);
    osDelay(500);
    BRT38_SendCmd(CAN_handle, ID, BRT38_MODE_SET, BRT38_MODE_VAL);
    osDelay(500);
    BRT38_SendCmd(CAN_handle, ID, BRT38_INC_DIRCT_SET, BRT38_INC_DIRCT_CW);
    osDelay(500);
    BRT38_SendCmd(CAN_handle, ID, BRT38_POS_0_SET, 0);
    osDelay(500);
}

#ifdef CAN_SUPPORT
#elif defined FDCAN_SUPPORT
__weak void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
    uint8_t RxFifo0[7];
    if (hfdcan->Instance == FDCAN1)
    {
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxFifo0);

        static struct BRT38_msg_t BRT38_msg_fdbk;

        memcpy(&BRT38_msg_fdbk, RxFifo0, RxFifo0[0]);

        static int16_t BRT38_lap;
        static float BRT38_angle_curr;

        switch (BRT38_msg_fdbk.func)
        {
        case BRT38_VAL_READ:
        {
            static float BRT38_angle_prev;
#ifdef BRT38_LAP_SGL

            BRT38_angle_curr = *(uint32_t *)BRT38_msg_fdbk.data * BRT38_fANGLE;

            if (ABS(BRT38_angle_curr - BRT38_angle_prev) >= 180)
                BRT38_angle += (BRT38_angle_curr > BRT38_angle_prev ? -180 : 180);
            BRT38_angle_prev = BRT38_angle_curr;

#endif
#ifdef BRT38_LAP_MPL

            BRT38_angle_curr = *(uint32_t *)BRT38_msg_fdbk.data * BRT38_fANGLE;

            if (ABS(BRT38_angle_curr - BRT38_angle_prev) >= BRT38_LAP * 180)
                BRT38_angle += (BRT38_angle_curr > BRT38_angle_prev ? -180 : 180) * BRT38_LAP;
            BRT38_angle_prev = BRT38_angle_curr;

#endif
            break;
        }
#ifdef DEBUG
        default:
        {
            if (BRT38_msg_fdbk.data[0])
                BRT38_status = BRT38_ERROR;
            else
                BRT38_status = BRT38_SUCCESS;
        }
#endif
        }
    }
}
#endif