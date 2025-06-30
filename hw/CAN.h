#ifndef __CAN_H
#define __CAN_H

#ifdef FDCAN_SUPPORT
#define CAN_FLTR_TYPE_STD_RANGE 0x00000000
#define CAN_FLTR_TYPE_STD_DUAL 0x40000000
#define CAN_FLTR_TYPE_STD_MASK 0x80000000
#define CAN_FLTR_TYPE_STD_DISABLED 0xC0000000

#define CAN_FLTR_CFG_STD_DISABLED 0x00000000
#define CAN_FLTR_CFG_STD_FIFO0 0x08000000
#define CAN_FLTR_CFG_STD_FIFO1 0x10000000
#define CAN_FLTR_CFG_STD_REJECT 0x18000000
//...
#elif defined CAN_SUPPORT
#define CAN_FLTR_ID_DATA_STD(CAN_ID) (CAN_ID) << 5 | CAN_ID_STD | CAN_RTR_DATA
#define CAN_FLTR_ID_DATA_EXT_H(CAN_ID) (CAN_ID) >> 13
#define CAN_FLTR_ID_DATA_EXT_L(CAN_ID) (CAN_ID) << 3 | CAN_ID_EXT | CAN_RTR_DATA

#define CAN_FLTR_MASK_DATA_STD(CAN_ID) (CAN_ID) << 5 | 0x001F
#define CAN_FLTR_MASK_DATA_EXT_H(CAN_ID) (CAN_ID) >> 13
#define CAN_FLTR_MASK_DATA_EXT_L(CAN_ID) (CAN_ID) << 3 | 0x7
#endif

#ifdef CAN_SUPPORT
static inline void CAN_SendData(CAN_HandleTypeDef *hcan, unsigned CAN_ID_Type, unsigned ID, unsigned char TxData[], unsigned char length)
{
    CAN_TxHeaderTypeDef CAN_TxHeader = {
        .StdId = CAN_ID_Type == CAN_ID_STD ? ID : 0,
        .ExtId = CAN_ID_Type == CAN_ID_STD ? 0 : ID,
        .IDE = CAN_ID_Type,
        .RTR = CAN_RTR_DATA,
        .DLC = length,
        .TransmitGlobalTime = 0};

    if (!HAL_CAN_IsTxMessagePending(hcan, CAN_TX_MAILBOX0))
        HAL_CAN_AddTxMessage(hcan, &CAN_TxHeader, TxData, (unsigned *)CAN_TX_MAILBOX0);
    else if (!HAL_CAN_IsTxMessagePending(hcan, CAN_TX_MAILBOX1))
        HAL_CAN_AddTxMessage(hcan, &CAN_TxHeader, TxData, (unsigned *)CAN_TX_MAILBOX1);
    else if (!HAL_CAN_IsTxMessagePending(hcan, CAN_TX_MAILBOX2))
        HAL_CAN_AddTxMessage(hcan, &CAN_TxHeader, TxData, (unsigned *)CAN_TX_MAILBOX2);
}

#elif defined FDCAN_SUPPORT
static inline void CAN_SendData(FDCAN_HandleTypeDef *hfdcan, unsigned FDCAN_IDType, unsigned ID, unsigned char TxData[], unsigned char length)
{
    FDCAN_TxHeaderTypeDef FDCAN_TxHeader = {
        .Identifier = ID,
        .IdType = FDCAN_IDType,
        .TxFrameType = FDCAN_DATA_FRAME,
        .DataLength = length,
        .ErrorStateIndicator = FDCAN_ESI_PASSIVE,
        .BitRateSwitch = FDCAN_BRS_OFF,
        .FDFormat = FDCAN_CLASSIC_CAN,
        .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
        .MessageMarker = 0};

    HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &FDCAN_TxHeader, TxData);
}

static inline void FDCAN_nBRS_SendData(FDCAN_HandleTypeDef *hfdcan, unsigned FDCAN_IDType, unsigned ID, unsigned char TxData[], unsigned FDCAN_DLC)
{
    FDCAN_TxHeaderTypeDef FDCAN_TxHeader = {
        .Identifier = ID,
        .IdType = FDCAN_IDType,
        .TxFrameType = FDCAN_DATA_FRAME,
        .DataLength = FDCAN_DLC,
        .ErrorStateIndicator = FDCAN_ESI_PASSIVE,
        .BitRateSwitch = FDCAN_BRS_OFF,
        .FDFormat = FDCAN_FD_CAN,
        .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
        .MessageMarker = 0};

    HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &FDCAN_TxHeader, TxData);
}

static inline void FDCAN_BRS_SendData(FDCAN_HandleTypeDef *hfdcan, unsigned FDCAN_IDType, unsigned ID, unsigned char TxData[], unsigned FDCAN_DLC)
{
    FDCAN_TxHeaderTypeDef FDCAN_TxHeader = {
        .Identifier = ID,
        .IdType = FDCAN_IDType,
        .TxFrameType = FDCAN_DATA_FRAME,
        .DataLength = FDCAN_DLC,
        .ErrorStateIndicator = FDCAN_ESI_PASSIVE,
        .BitRateSwitch = FDCAN_BRS_ON,
        .FDFormat = FDCAN_FD_CAN,
        .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
        .MessageMarker = 0};

    HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &FDCAN_TxHeader, TxData);
}
#endif
#endif