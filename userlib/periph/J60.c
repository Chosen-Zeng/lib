#include "main.h"
#include "algorithm.h"
#include "J60.h"
#include "J60_INST.h"
#ifdef CAN_SUPPORT
#include "CAN.h"
#elif defined FDCAN_SUPPORT
#include "FDCAN.h"
#endif

J60_CTRL_t J60_CTRL;
J60_FDBK_t J60_FDBK;

void J60_SendCMD(void *CAN_handle, uint8_t ID, uint16_t J60_CMD, float data)
{
    uint8_t TxData[8], length = 0;
    switch (J60_CMD)
    {
    case J60_MOTOR_CONTROL:
    {
        length = 8;

        ABS_LIMIT(J60_CTRL.angle, J60_ANGLE_LIMIT)
        *(uint64_t *)TxData = (uint16_t)((J60_CTRL.angle + J60_ANGLE_LIMIT) / J60_fANGLE_W);

        ABS_LIMIT(J60_CTRL.spd, J60_SPD_LIMIT)
        *(uint64_t *)TxData |= (uint64_t)((J60_CTRL.spd + J60_SPD_LIMIT) / J60_fSPD_W) << 16;

        LIMIT(J60_CTRL.Kp, J60_Kp_LIMIT)
        *(uint64_t *)TxData |= (uint64_t)J60_CTRL.Kp << 30;

        LIMIT(J60_CTRL.Kd, J60_Kd_LIMIT)
        *(uint64_t *)TxData |= (uint64_t)(J60_CTRL.Kd / J60_fKd) << 40;

        ABS_LIMIT(J60_CTRL.torque, J60_TORQUE_LIMIT)
        *(uint64_t *)TxData |= (uint64_t)((J60_CTRL.torque + J60_TORQUE_LIMIT) / J60_fTORQUE) << 48;
        break;
    }
    case J60_SET_CAN_TIMEOUT:
    {
        length = 1;

        LIMIT(data, J60_CAN_TIMEOUT_LIMIT)
        TxData[0] = (uint8_t)data;
        break;
    }
    case J60_SET_BANDWIDTH:
    {
        length = 2;

        LIMIT(data, J60_BANDWIDTH_LIMIT)
        *(uint16_t *)TxData = (uint16_t)data;
        break;
    }
    }
#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_STD, J60_CMD | ID, TxData, length);
#elif defined FDCAN_SUPPORT
    FDCAN_SendData();
#endif
}

//void J60_Init(void *CAN_handle, uint8_t ID)