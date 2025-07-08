#include "VESC.h"
#include "algorithm.h"
#include "CAN.h"

#ifdef VESC_NUM

motor_info_t T_MOTOR_AT4130_KV450 = {.curr_max = 75, .PP = 7},
             HOBBYWING_V9626_KV160 = {.curr_max = 171.5, .PP = 21},
             CUBEMARS_R100_KV90 = {.curr_max = 90.5, .PP = 21},
             CUBEMARS_RO100_KV55 = {.curr_max = 62, .PP = 21};

void VESC_SendCmd(CAN_handle_t *const CAN_handle, const unsigned char arrID, const unsigned short VESC_cmd)
{
    unsigned char TxData[4];

    switch (VESC_cmd)
    {
    case VESC_SET_CURR:
    case VESC_SET_CURR_BRAKE:
    {
        f_to_u8_rev(LIMIT_ABS(VESC[arrID].ctrl.curr, VESC[arrID].motor->curr_max) * VESC_fCURR_W, TxData);
        break;
    }
    case VESC_SET_SPD:
    {
        f_to_u8_rev(VESC[arrID].ctrl.spd * VESC[arrID].motor->PP, TxData);
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
    CAN_SendData(CAN_handle, CAN_ID_EXT, VESC_cmd << 8 | VESC[arrID].ID, TxData, 4);
#elif defined FDCAN_SUPPORT
    CAN_SendData(CAN_handle, FDCAN_EXTENDED_ID, VESC_cmd << 8 | VESC[arrID].ID, TxData, 4);
#endif
}

bool VESC_MsgHandler(const unsigned CAN_ID, const unsigned char arrID, const unsigned char RxData[8])
{
    if (CAN_ID == (VESC_STATUS_1 << 8 | VESC[arrID].ID))
    {
        VESC[arrID].fdbk.spd = (float)(int)(RxData[0] << 24 | RxData[1] << 16 | RxData[2] << 8 | RxData[3]) / VESC[arrID].motor->PP;
        VESC[arrID].fdbk.curr = (float)(short)(RxData[4] << 8 | RxData[5]) / VESC_fCURR_R;
        VESC[arrID].fdbk.dutycycle = (float)(short)(RxData[6] << 8 | RxData[7]) / VESC_fPCT_R;

        return true;
    }
    else if (CAN_ID == (VESC_STATUS_4 << 8 | VESC[arrID].ID))
    {
        VESC[arrID].fdbk.temp.MOSFET = (float)(short)(RxData[0] << 8 | RxData[1]) / VESC_fTEMP;
        VESC[arrID].fdbk.curr_in = (float)(short)(RxData[4] << 8 | RxData[5]) / VESC_fCURR_R;
        VESC[arrID].fdbk.pos = (float)(short)(RxData[6] << 8 | RxData[7]) / VESC_fPOS_R;

        return true;
    }
    else if (CAN_ID == (VESC_STATUS_5 << 8 | VESC[arrID].ID))
    {
        VESC[arrID].fdbk.volt = (float)(short)(RxData[4] << 8 | RxData[5]) / VESC_fVOLT;

        return true;
    }
    return false;
}
#endif