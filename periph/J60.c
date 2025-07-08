#include "J60.h"
#include "algorithm.h"
#include "CAN.h"

#ifdef J60_NUM

void J60_SendCmd(CAN_handle_t *const CAN_handle, const unsigned char arrID, const unsigned short J60_cmd, float data)
{
    unsigned char TxData[8], len = 0;

    switch (J60_cmd)
    {
    case J60_MOTOR_CONTROL:
    {
        len = 8;

        *(unsigned long *)TxData = (unsigned long)(LIMIT_ABS(J60[arrID].ctrl.pos, J60_POS_LIMIT) + J60_POS_LIMIT) / J60_fPOS_W;
        *(unsigned long *)TxData |= (unsigned long)(LIMIT_ABS(J60[arrID].ctrl.spd, J60_SPD_LIMIT) + J60_SPD_LIMIT) / J60_fSPD_W << 16;
        *(unsigned long *)TxData |= (unsigned long)LIMIT(J60[arrID].ctrl.Kp, J60_Kp_LIMIT) << 30;
        *(unsigned long *)TxData |= (unsigned long)LIMIT(J60[arrID].ctrl.Kd, J60_Kd_LIMIT) / J60_fKd << 40;
        *(unsigned long *)TxData |= (unsigned long)(LIMIT_ABS(J60[arrID].ctrl.trq, J60_TORQUE_LIMIT) + J60_TORQUE_LIMIT) / J60_fTORQUE << 48;

        break;
    }
    case J60_SET_CAN_TIMEOUT:
    {
        len = 1;

        TxData[0] = LIMIT(data, J60_CAN_TIMEOUT_LIMIT);
        break;
    }
    case J60_SET_BANDWIDTH:
    {
        len = 2;

        *(unsigned short *)TxData = LIMIT(data, J60_BANDWIDTH_LIMIT);
        break;
    }
    }
#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_STD, J60_cmd << 5 | J60_MSG_SEND << 4 | J60[arrID].ID, TxData, len);
#elif defined FDCAN_SUPPORT
    CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, J60_cmd << 5 | J60_MSG_SEND << 4 | J60[arrID].ID, TxData, len);
#endif
}

void J60_Init(CAN_handle_t *const CAN_handle, const unsigned char arrID)
{
    J60_SendCmd(CAN_handle, arrID, J60_MOTOR_ENABLE, 0);
}

bool J60_MsgHandler(const unsigned CAN_ID, const unsigned char arrID, const unsigned char RxData[8])
{
    if (CAN_ID == (J60_MOTOR_CONTROL << 5 | J60_MSG_RECV << 4 | J60[arrID].ID))
    {
        J60[arrID].fdbk.pos = (*(unsigned *)RxData & 0xFFFFF) * J60_fPOS_R - J60_POS_LIMIT;
        J60[arrID].fdbk.spd = (*(unsigned *)&RxData[2] >> 4 & 0xFFFFF) * J60_fSPD_R - J60_SPD_LIMIT;
        J60[arrID].fdbk.trq = *(unsigned short *)&RxData[5] * J60_fTORQUE - J60_TORQUE_LIMIT;
        if ((*(unsigned char *)&RxData[7] & 1) == J60_TEMP_FLAG_MOSFET)
            J60[arrID].fdbk.temp.MOSFET = (*(unsigned char *)&RxData[7] >> 1) * J60_fTEMP + J60_TEMP_OFFSET;
        else
            J60[arrID].fdbk.temp.motor = (*(unsigned char *)&RxData[7] >> 1) * J60_fTEMP + J60_TEMP_OFFSET;

        return true;
    }
    return false;
}

#else
#error J60_NUM undefined
#endif