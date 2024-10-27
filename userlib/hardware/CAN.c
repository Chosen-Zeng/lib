#include "CAN.h"

void CAN_SendData(CAN_HandleTypeDef *hcan, uint32_t ID, uint8_t TxData[], uint8_t length)
{
  CAN_TxHeaderTypeDef TxHeader;
  TxHeader.StdId = ID;
  TxHeader.ExtId = 0;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.DLC = 8;
  TxHeader.TransmitGlobalTime = 0;

  while (!HAL_CAN_GetTxMailboxesFreeLevel(hcan))
    ;

  if (!HAL_CAN_IsTxMessagePending(hcan, CAN_TX_MAILBOX0))
    HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, (uint32_t *)CAN_TX_MAILBOX0);
  else if (!HAL_CAN_IsTxMessagePending(hcan, CAN_TX_MAILBOX1))
    HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, (uint32_t *)CAN_TX_MAILBOX1);
  else if (!HAL_CAN_IsTxMessagePending(hcan, CAN_TX_MAILBOX2))
    HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, (uint32_t *)CAN_TX_MAILBOX2);
}