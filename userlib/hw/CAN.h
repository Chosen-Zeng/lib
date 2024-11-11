#ifndef __CAN_H
#define __CAN_H

#include "main.h"

void CAN_SendData(CAN_HandleTypeDef* hcan, uint32_t CAN_ID_Type, uint32_t ID, uint8_t TxData[], uint8_t length);

#endif