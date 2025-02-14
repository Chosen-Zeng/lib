#include "VESC.h"
#include "algorithm.h"
#include "CAN.h"

#if defined VESC_NUM && defined VESC_ID_OFFSET

VESC_t VESC[VESC_NUM];

void VESC_SendCmd(void *CAN_handle, unsigned char ID, unsigned short VESC_cmd)
{
    unsigned char TxData[4];

    switch (VESC_cmd)
    {
    case VESC_SET_CURR:
    {
        f_2_4u8(LIMIT_ABS(VESC[ID - VESC_ID_OFFSET].ctrl.curr, VESC_MOTOR_CURR_MAX) * VESC_fCURR_W, TxData);
        break;
    }
    case VESC_SET_SPD:
    {
        f_2_4u8(LIMIT_ABS(VESC[ID - VESC_ID_OFFSET].ctrl.spd, VESC_MOTOR_SPD_MAX) * VESC_MOTOR_PP, TxData);
        break;
    }
    case VESC_SET_POS:
    {
        f_2_4u8(LIMIT(VESC[ID - VESC_ID_OFFSET].ctrl.pos, VESC_POS_MAX) * VESC_fPOS_W, TxData);
        break;
    }
    default:
        return;
    }
#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_EXT, VESC_cmd | ID, TxData, 4);
#elif defined FDCAN_SUPPORT
    CAN_SendData(CAN_handle, FDCAN_EXTENDED_ID, VESC_cmd | ID, TxData, 4);
#endif
}

/*
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    unsigned char RxFifo0[8];
    FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxFifo0);

    if (hfdcan->Instance == FDCAN1)
    {
        switch ((FDCAN_RxHeader.Identifier & 0xFF00) >> 8)
        {
        case VESC_STATUS:
        {
            VESC[(FDCAN_RxHeader.Identifier & 0x00FF) - VESC_ID_OFFSET].fdbk.spd = (float)(RxFifo0[0] << 24 | RxFifo0[1] << 16 | RxFifo0[2] << 8 | RxFifo0[3]) / VESC_MOTOR_PP;
            break;
        }
        case VESC_STATUS_4:
        {
            VESC[(FDCAN_RxHeader.Identifier & 0x00FF) - VESC_ID_OFFSET].fdbk.curr_in = (float)(RxFifo0[4] << 8 | RxFifo0[5]) / VESC_fCURR_R;
            VESC[(FDCAN_RxHeader.Identifier & 0x00FF) - VESC_ID_OFFSET].fdbk.pos = (float)(RxFifo0[6] << 8 | RxFifo0[7]) / VESC_fPOS_R;
            break;
        }
        }
    }
}
*/
#endif