#ifndef __CAN_H
#define __CAN_H

#include "user.h"

#define CAN_FILTER_ID_DATA_STD(CAN_ID) (CAN_ID) << 5 | CAN_ID_STD | CAN_RTR_DATA
#define CAN_FILTER_ID_DATA_EXT_H(CAN_ID) (CAN_ID) >> 13
#define CAN_FILTER_ID_DATA_EXT_L(CAN_ID) (CAN_ID) << 3 | CAN_ID_EXT | CAN_RTR_DATA

#define CAN_FILTER_MASK_DATA_STD(CAN_ID) (CAN_ID) << 5 | 0x001F
#define CAN_FILTER_MASK_DATA_EXT_H(CAN_ID) (CAN_ID) >> 13
#define CAN_FILTER_MASK_DATA_EXT_L(CAN_ID) (CAN_ID) << 3 | 0x7

static inline void CAN_SendData(CAN_HandleTypeDef *hcan, uint32_t CAN_ID_Type, uint32_t ID, uint8_t TxData[], uint8_t length)
{
    CAN_TxHeaderTypeDef CAN_TxHeader;

    switch (CAN_ID_Type)
    {
    case CAN_ID_STD:
    {
        CAN_TxHeader.StdId = ID;
        CAN_TxHeader.ExtId = 0;
        break;
    }
    case CAN_ID_EXT:
    {
        CAN_TxHeader.StdId = 0;
        CAN_TxHeader.ExtId = ID;
        break;
    }
    }

    CAN_TxHeader.IDE = CAN_ID_Type;
    CAN_TxHeader.RTR = CAN_RTR_DATA;
    CAN_TxHeader.DLC = length;
    CAN_TxHeader.TransmitGlobalTime = 0;

    if (!HAL_CAN_IsTxMessagePending(hcan, CAN_TX_MAILBOX0))
        HAL_CAN_AddTxMessage(hcan, &CAN_TxHeader, TxData, (uint32_t *)CAN_TX_MAILBOX0);
    else if (!HAL_CAN_IsTxMessagePending(hcan, CAN_TX_MAILBOX1))
        HAL_CAN_AddTxMessage(hcan, &CAN_TxHeader, TxData, (uint32_t *)CAN_TX_MAILBOX1);
    else if (!HAL_CAN_IsTxMessagePending(hcan, CAN_TX_MAILBOX2))
        HAL_CAN_AddTxMessage(hcan, &CAN_TxHeader, TxData, (uint32_t *)CAN_TX_MAILBOX2);
}

#endif