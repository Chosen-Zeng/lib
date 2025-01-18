#ifndef __FDCAN_H
#define __FDCAN_H

#include "user.h"

void CAN_SendData(FDCAN_HandleTypeDef *hfdcan, uint32_t FDCAN_IDType, uint32_t ID, uint8_t TxData[], uint8_t length);
void FDCAN_nBRS_SendData(FDCAN_HandleTypeDef *hfdcan, uint32_t FDCAN_IDType, uint32_t ID, uint8_t TxData[], uint32_t FDCAN_DLC);
void FDCAN_BRS_SendData(FDCAN_HandleTypeDef *hfdcan, uint32_t FDCAN_IDType, uint32_t ID, uint8_t TxData[], uint32_t FDCAN_DLC);

#endif