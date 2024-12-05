#ifndef __CAN_H
#define __CAN_H

#include "main.h"

#define CAN_FILTER_ID_DATA_STD(CAN_ID) (CAN_ID) << 5 | CAN_ID_STD | CAN_RTR_DATA
#define CAN_FILTER_ID_DATA_EXT_H(CAN_ID) (CAN_ID) >> 13
#define CAN_FILTER_ID_DATA_EXT_L(CAN_ID) (CAN_ID) << 3 | CAN_ID_EXT | CAN_RTR_DATA

#define CAN_FILTER_MASK_DATA_STD(CAN_ID) (CAN_ID) << 5 | 0x001F
#define CAN_FILTER_MASK_DATA_EXT_H(CAN_ID) (CAN_ID) >> 13
#define CAN_FILTER_MASK_DATA_EXT_L(CAN_ID) (CAN_ID) << 3 | 0x7

void CAN_SendData(CAN_HandleTypeDef *hcan, uint32_t CAN_ID_Type, uint32_t ID, uint8_t TxData[], uint8_t length);

#endif