#include "main.h"
#include "VESC.h"
#include "VESC_INST.h"
#include "algorithm.h"
#ifdef CAN_SUPPORT
#include "CAN.h"
#elif defined FDCAN_SUPPORT
#include "FDCAN.h"
#endif

#if defined VESC_NUM && defined VESC_ID_OFFSET

VESC_t VESC[VESC_NUM];

void f_2_4u8(float num, uint8_t arr[4])
{
    for (int count = 0; count < 4; count++)
    {
        arr[count] = (int32_t)num >> 8 * (3 - count);
    }
}

void VESC_SendCmd(void *CAN_handle, uint8_t ID, uint8_t VESC_cmd)
{
    uint8_t TxData[4];

    switch (VESC_cmd)
    {
    case VESC_SET_CURRENT:
    {
        LIMIT_ABS(VESC[ID - VESC_ID_OFFSET].ctrl.curr, VESC_MOTOR_CURRENT_MAX);
        f_2_4u8(VESC[ID - VESC_ID_OFFSET].ctrl.curr * VESC_fCURRENT_W, TxData);
        break;
    }
    case VESC_SET_RPM:
    {
        LIMIT_ABS(VESC[ID - VESC_ID_OFFSET].ctrl.spd, VESC_MOTOR_RPM_MAX);
        f_2_4u8(VESC[ID - VESC_ID_OFFSET].ctrl.spd * VESC_MOTOR_PP, TxData);
        break;
    }
    }
#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_EXT, ID | VESC_cmd << 8, TxData, 4);
#elif defined FDCAN_SUPPORT
    FDCAN_SendData(CAN_handle, FDCAN_EXTENDED_ID, ID | VESC_cmd << 8, TxData, 4);
#endif
}

/*
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    uint8_t RxFifo0[8];
    FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxFifo0);

    if (hfdcan->Instance == FDCAN1)
    {
        switch ((FDCAN_RxHeader.Identifier & 0xFF00) >> 8)
        {
        case VESC_STATUS:
        {
            VESC_fdbk[(FDCAN_RxHeader.Identifier & 0x00FF) - VESC_ID_OFFSET].spd = (float)(RxFifo0[0] << 24 | RxFifo0[1] << 16 | RxFifo0[2] << 8 | RxFifo0[3]) / VESC_MOTOR_PP;
            break;
        }
        case VESC_STATUS_4:
        {
            VESC_fdbk[(FDCAN_RxHeader.Identifier & 0x00FF) - VESC_ID_OFFSET].curr_in = (float)(RxFifo0[4] << 8 | RxFifo0[5]) / VESC_fCURRENT_R;
            break;
        }
        }
    }
}
*/
#endif