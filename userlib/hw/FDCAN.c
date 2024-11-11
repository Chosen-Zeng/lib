#include "FDCAN.h"


void FDCAN_SendData(FDCAN_HandleTypeDef* hfdcan, uint32_t IDType, uint32_t ID, uint8_t TxData[], uint8_t length)
{
	FDCAN_TxHeaderTypeDef FDCAN_TxHeader;
	FDCAN_TxHeader.Identifier = ID;
	FDCAN_TxHeader.IdType = IDType;
	FDCAN_TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	FDCAN_TxHeader.DataLength = length;
	FDCAN_TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	FDCAN_TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
	FDCAN_TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
	FDCAN_TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	FDCAN_TxHeader.MessageMarker = 0;
	
	while (!HAL_FDCAN_GetTxFifoFreeLevel(hfdcan))
		;
	
	HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &FDCAN_TxHeader, TxData);
}