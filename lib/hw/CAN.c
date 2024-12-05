#include "CAN.h"

void CAN_SendData(CAN_HandleTypeDef *hcan, uint32_t CAN_ID_Type, uint32_t ID, uint8_t TxData[], uint8_t length)
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

  //  while (!HAL_CAN_GetTxMailboxesFreeLevel(hcan))
  //    ;

  if (!HAL_CAN_IsTxMessagePending(hcan, CAN_TX_MAILBOX0))
    HAL_CAN_AddTxMessage(hcan, &CAN_TxHeader, TxData, (uint32_t *)CAN_TX_MAILBOX0);
  else if (!HAL_CAN_IsTxMessagePending(hcan, CAN_TX_MAILBOX1))
    HAL_CAN_AddTxMessage(hcan, &CAN_TxHeader, TxData, (uint32_t *)CAN_TX_MAILBOX1);
  else if (!HAL_CAN_IsTxMessagePending(hcan, CAN_TX_MAILBOX2))
    HAL_CAN_AddTxMessage(hcan, &CAN_TxHeader, TxData, (uint32_t *)CAN_TX_MAILBOX2);
}