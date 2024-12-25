#include "algorithm.h"
#include "J60.h"
#include "J60_INST.h"
#ifdef CAN_SUPPORT
#include "CAN.h"
#elif defined FDCAN_SUPPORT
#include "FDCAN.h"
#endif

#if defined J60_NUM && defined J60_ID_OFFSET
J60_t J60[J60_NUM];

void J60_SendCmd(void *CAN_handle, uint8_t ID, uint16_t J60_cmd, float data)
{
    uint8_t TxData[8], length = 0;

    switch (J60_cmd)
    {
    case J60_MOTOR_CONTROL:
    {
        length = 8;
        uint8_t ID_array = ID - J60_ID_OFFSET;

        LIMIT_ABS(J60[ID_array].ctrl.angle, J60_ANGLE_LIMIT)
        *(uint64_t *)TxData = (uint64_t)((J60[ID_array].ctrl.angle + J60_ANGLE_LIMIT) / J60_fANGLE_W);

        LIMIT_ABS(J60[ID_array].ctrl.spd, J60_SPD_LIMIT)
        *(uint64_t *)TxData |= (uint64_t)((J60[ID_array].ctrl.spd + J60_SPD_LIMIT) / J60_fSPD_W) << 16;

        LIMIT(J60[ID_array].ctrl.Kp, J60_Kp_LIMIT)
        *(uint64_t *)TxData |= (uint64_t)J60[ID_array].ctrl.Kp << 30;

        LIMIT(J60[ID_array].ctrl.Kd, J60_Kd_LIMIT)
        *(uint64_t *)TxData |= (uint64_t)(J60[ID_array].ctrl.Kd / J60_fKd) << 40;

        LIMIT_ABS(J60[ID_array].ctrl.torque, J60_TORQUE_LIMIT)
        *(uint64_t *)TxData |= (uint64_t)((J60[ID_array].ctrl.torque + J60_TORQUE_LIMIT) / J60_fTORQUE) << 48;
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
    CAN_SendData(CAN_handle, CAN_ID_STD, J60_cmd | ID, TxData, length);
#elif defined FDCAN_SUPPORT
    FDCAN_SendData(CAN_handle, FDCAN_STANDARD_ID, J60_cmd | ID, TxData, length);
#endif
}

void J60_Init(void *CAN_handle, uint8_t ID)
{
    J60_SendCmd(CAN_handle, ID, J60_MOTOR_ENABLE, 0);
}

#ifdef CAN_SUPPORT
/*void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)//__weak 
{
    CAN_RxHeaderTypeDef CAN_RxHeader;
    uint8_t RxFifo1[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &CAN_RxHeader, RxFifo1);

    if (hcan->Instance == CAN1)
    {
        switch (CAN_RxHeader.StdId & 0x7E0)
        {
        case J60_MOTOR_CONTROL:
        {
            uint8_t ID_array = (CAN_RxHeader.StdId & 0xF) - J60_ID_OFFSET;

            J60[ID_array].fdbk.angle = (*(uint32_t *)RxFifo1 & 0xFFFFF) * J60_fANGLE_R - J60_ANGLE_LIMIT;
            J60[ID_array].fdbk.spd = (*(uint32_t *)&RxFifo1[2] >> 4 & 0xFFFFF) * J60_fSPD_R - J60_SPD_LIMIT;
            J60[ID_array].fdbk.torque = *(uint16_t *)&RxFifo1[5] * J60_fTORQUE - J60_TORQUE_LIMIT;
            if (J60[ID_array].fdbk.temp_flag = *(uint8_t *)&RxFifo1[7] & 1)
                J60[ID_array].fdbk.temp_motor = (*(uint8_t *)&RxFifo1[7] >> 1) * J60_fTEMP + J60_TEMP_OFFSET;
            else
                J60[ID_array].fdbk.temp_MOSFET = (*(uint8_t *)&RxFifo1[7] >> 1) * J60_fTEMP + J60_TEMP_OFFSET;

            break;
        }
        }
    }
}*/
#elif defined FDCAN_SUPPORT
#endif
#endif