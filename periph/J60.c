#include "J60.h"
#include "algorithm.h"
#include "CAN.h"

#if defined J60_NUM && defined J60_ID_OFFSET
J60_t J60[J60_NUM];

void J60_SendCmd(void *CAN_handle, unsigned char ID, unsigned short J60_cmd, float data)
{
    unsigned char TxData[8], len = 0;

    switch (J60_cmd)
    {
    case J60_MOTOR_CONTROL:
    {
        len = 8;
        unsigned char ID_array = ID - J60_ID_OFFSET;

        *(unsigned long *)TxData = (unsigned long)(LIMIT_ABS(J60[ID_array].ctrl.pos, J60_POS_LIMIT) + J60_POS_LIMIT) / J60_fPOS_W;
        *(unsigned long *)TxData |= (unsigned long)(LIMIT_ABS(J60[ID_array].ctrl.spd, J60_SPD_LIMIT) + J60_SPD_LIMIT) / J60_fSPD_W << 16;
        *(unsigned long *)TxData |= (unsigned long)LIMIT(J60[ID_array].ctrl.Kp, J60_Kp_LIMIT) << 30;
        *(unsigned long *)TxData |= (unsigned long)LIMIT(J60[ID_array].ctrl.Kd, J60_Kd_LIMIT) / J60_fKd << 40;
        *(unsigned long *)TxData |= (unsigned long)(LIMIT_ABS(J60[ID_array].ctrl.trq, J60_TORQUE_LIMIT) + J60_TORQUE_LIMIT) / J60_fTORQUE << 48;

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
    CAN_SendData(CAN_handle, CAN_ID_STD, J60_cmd | ID, TxData, len);
#elif defined FDCAN_SUPPORT
    CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, J60_cmd | ID, TxData, len);
#endif
}

void J60_Init(void *CAN_handle, unsigned char ID)
{
    J60_SendCmd(CAN_handle, ID, J60_MOTOR_ENABLE, 0);
}

#ifdef CAN_SUPPORT
/*void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)//__weak
{
    CAN_RxHeaderTypeDef CAN_RxHeader;
    unsigned char RxFifo1[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &CAN_RxHeader, RxFifo1);

    if (hcan->Instance == CAN1)
    {
        switch (CAN_RxHeader.StdId & 0x7E0)
        {
        case J60_MOTOR_CONTROL:
        {
            unsigned char ID_array = (CAN_RxHeader.StdId & 0xF) - J60_ID_OFFSET;

            J60[ID_array].fdbk.pos = (*(uint32_t *)RxFifo1 & 0xFFFFF) * J60_fPOS_R - J60_POS_LIMIT;
            J60[ID_array].fdbk.spd = (*(uint32_t *)&RxFifo1[2] >> 4 & 0xFFFFF) * J60_fSPD_R - J60_SPD_LIMIT;
            J60[ID_array].fdbk.trq = *(unsigned short *)&RxFifo1[5] * J60_fTORQUE - J60_TORQUE_LIMIT;
            if (J60[ID_array].fdbk.temp_flag = *(unsigned char *)&RxFifo1[7] & 1)
                J60[ID_array].fdbk.temp_motor = (*(unsigned char *)&RxFifo1[7] >> 1) * J60_fTEMP + J60_TEMP_OFFSET;
            else
                J60[ID_array].fdbk.temp_MOSFET = (*(unsigned char *)&RxFifo1[7] >> 1) * J60_fTEMP + J60_TEMP_OFFSET;

            break;
        }
        }
    }
}*/
#elif defined FDCAN_SUPPORT
#endif
#endif