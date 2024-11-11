#include "main.h"
#include "cmsis_os.h"
#include "BRT38.h"
#include "BRT38_INST.h"
#ifdef CAN_SUPPORT
#include "CAN.h"
#elif defined FDCAN_SUPPORT
#include "FDCAN.h"
#endif

#define ABS(X) ((X) >= 0 ? (X) : -(X)) // 输出X绝对值

BRT38_t BRT38_CTRL;
float BRT38_angle;

#ifdef DEBUG
uint8_t BRT38_status;
#endif

static void BRT38_SendData(void *CAN_handle, uint8_t addr, uint8_t func, uint32_t data, uint8_t datalength)
{
    BRT38_CTRL.length = datalength + 3;
    BRT38_CTRL.addr = addr;
    BRT38_CTRL.func = func;
    *(uint32_t *)BRT38_CTRL.data = data;

#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_STD, BRT38_CTRL.addr, &BRT38_CTRL, BRT38_CTRL.length);
#elif defined FDCAN_SUPPROT
    FDCAN_SendData(CAN_handle, FDCAN_STANDARD_ID, BRT38_CTRL.addr, &BRT38_CTRL, BRT38_CTRL.length);
#endif
}

void BRT38_Init(void *CAN_handle, uint8_t ID)
{
    BRT38_SendData(CAN_handle, ID, BRT38_ATD_SET, 1000, 2);
    osDelay(500);
    BRT38_SendData(CAN_handle, ID, BRT38_MODE_SET, BRT38_MODE_VAL, 1);
    osDelay(500);
    BRT38_SendData(CAN_handle, ID, BRT38_INC_DIRCT_SET, BRT38_INC_DIRCT_CW, 1);
    osDelay(500);
    BRT38_SendData(CAN_handle, ID, BRT38_ZP_SET, 0, 1);
    osDelay(500);
}


/*void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
    uint8_t RxFifo0[7];
    if (hfdcan->Instance == FDCAN1)
    {
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxFifo0);

        static struct BRT38 BRT38_FDBK;

        memcpy(&BRT38_FDBK, RxFifo0, RxFifo0[0]);

        static int16_t BRT38_lap;
        static float BRT38_angle_curr;

        switch (BRT38_FDBK.func)
        {
        case BRT38_VAL_READ:
        {
            static float BRT38_angle_prev;
#ifdef BRT38_LAP_SGL

            BRT38_angle_curr = *(uint32_t *)BRT38_FDBK.data * BRT38_fANGLE;

            if (ABS(BRT38_angle_curr - BRT38_angle_prev) > 180)
                BRT38_angle_curr > BRT38_angle_prev ? BRT38_lap-- : BRT38_lap++;
            BRT38_angle_prev = BRT38_angle_curr;

            BRT38_angle = BRT38_lap * 360 + BRT38_angle_curr;
#endif
#ifdef BRT38_LAP_MPL

            BRT38_angle_curr = *(uint32_t *)BRT38_FDBK.data * BRT38_fANGLE;

            if (ABS(BRT38_angle_curr - BRT38_angle_prev) > BRT38_LAP * 180)
                BRT38_angle_curr > BRT38_angle_prev ? BRT38_lap-- : BRT38_lap++;
            BRT38_angle_prev = BRT38_angle_curr;

            BRT38_angle = BRT38_lap * BRT38_LAP * 360 + BRT38_angle_curr;
#endif
            break;
        }
#ifdef DEBUG
        default:
        {
            if (BRT38_FDBK.data[0])
                BRT38_status = BRT38_ERROR;
            else
                BRT38_status = BRT38_SUCCESS;
        }
#endif
        }
    }
}*/