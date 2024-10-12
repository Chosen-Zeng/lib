#ifndef __FDCAN_H
#define __FDCAN_H


void FDCAN_SendData(FDCAN_HandleTypeDef* hfdcan, uint32_t ID, uint8_t TxData[]);

#endif