#include "VESC.h"
#include "algorithm.h"
#include "CAN.h"

#if defined VESC_NUM && defined VESC_ID_OFFSET

motor_info_t T_MOTOR_AT4130_KV450 = {.spd_max = 9000, .PP = 7},
             HOBBYWING_V9626_KV160 = {.curr_max = 171.5, .spd_max = 6000, .PP = 21},
             CUBEMARS_R100_KV90 = {.curr_max = 90.5, .spd_max = 3740, .PP = 21},
             CUBEMARS_RO100_KV55 = {.curr_max = 62, .spd_max = 2550, .PP = 21};

VESC_t VESC[VESC_NUM];

void VESC_SendCmd(void *CAN_handle, unsigned char ID, unsigned short VESC_cmd, motor_info_t *motor_info)
{
    unsigned char TxData[4],
        arrID = ID - VESC_ID_OFFSET;

    switch (VESC_cmd)
    {
    case VESC_SET_CURR:
    case VESC_SET_CURR_BRAKE:
    {
        f_to_u8_rev(VESC[arrID].ctrl.curr * VESC_fCURR_W, TxData);
        break;
    }
    case VESC_SET_SPD:
    {
        f_to_u8_rev(LIMIT_ABS(VESC[arrID].ctrl.spd, motor_info->spd_max) * motor_info->PP, TxData);
        break;
    }
    case VESC_SET_POS:
    {
        f_to_u8_rev(LIMIT(VESC[arrID].ctrl.pos, VESC_POS_MAX) * VESC_fPOS_W, TxData);
        break;
    }
    default:
        return;
    }
#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_EXT, VESC_cmd << 8 | ID, TxData, 4);
#elif defined FDCAN_SUPPORT
    CAN_SendData(CAN_handle, FDCAN_EXTENDED_ID, VESC_cmd << 8 | ID, TxData, 4);
#endif
}

/*
void FDCAN2_IT0_IRQHandler(void)
{
    if (FDCAN2->IR & 0x1)
    {
        FDCAN2->IR |= 0x1;

        FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
        unsigned char RxData[8];
        HAL_FDCAN_GetRxMessage(&hfdcan2, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxData);

        switch (FDCAN_RxHeader.Identifier)
        {
        case (VESC_STATUS_1 << 8 | ID):
        {
            VESC[1 - VESC_ID_OFFSET].fdbk.spd = (float)(RxData[0] << 24 | RxData[1] << 16 | RxData[2] << 8 | RxData[3]) / HOBBYWING_V9626_KV160.PP;
            VESC[1 - VESC_ID_OFFSET].fdbk.curr = (float)(RxData[4] << 8 | RxData[5]) / VESC_fCURR_R;
            break;
        }
        case (VESC_STATUS_5 << 8 | ID):
        {
            VESC[1 - VESC_ID_OFFSET].fdbk.volt = (float)(RxData[4] << 8 | RxData[5]);
            break;
        }
        }
    }
}
*/
#endif