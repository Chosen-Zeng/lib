#include "DaRan.h"

#if defined DARAN_NUM && defined DARAN_ID_OFFSET

DaRan_t DaRan[DARAN_NUM + 1];

void DaRan_CANFdbkInit(FDCAN_HandleTypeDef *hfdcan, unsigned char ID, float intvl_ms)
{
    DaRan_Prop_W(hfdcan, ID, DARAN_PARAM_CAN_FDBK_EN, DARAN_DATA_TYPE_u32, 1);
    DaRan_Prop_W(hfdcan, ID, DARAN_PARAM_CAN_FDBK_INTVL_ms, DARAN_DATA_TYPE_u32, intvl_ms);
}

// @note additional param needed in specific mode
void DaRan_SetPos(FDCAN_HandleTypeDef *hfdcan, unsigned char ID, unsigned char DARAN_CMD_POS_MODE, float param)
{
    unsigned char TxData[8];
    unsigned char ID_array = ID == DARAN_ID_BCAST ? DARAN_NUM : ID - DARAN_ID_OFFSET;

    *(float *)TxData = DaRan[ID_array].ctrl.pos;
    *(short *)&TxData[4] = LIMIT_RANGE(DaRan[ID_array].ctrl.spd, 32767 / DARAN_fSPD, -32768 / DARAN_fSPD) * DARAN_fSPD;

    switch (DARAN_CMD_POS_MODE)
    {
    case DARAN_CMD_POS_MODE_LIMIT:
    case DARAN_CMD_POS_MODE_FDFWD:
    {
        *(short *)&TxData[6] = LIMIT_RANGE(DaRan[ID_array].ctrl.trq, 32767 * DARAN_SCALING, -32768 * DARAN_SCALING) / DARAN_SCALING;
        break;
    }
    case DARAN_CMD_POS_MODE_FLTR:
    {
        *(short *)&TxData[6] = LIMIT(param, DARAN_FLTR_BW_MAX) / DARAN_SCALING;
        break;
    }
    case DARAN_CMD_POS_MODE_ACCEL:
    {
        *(short *)&TxData[6] = LIMIT_RANGE(param, 32767 * DARAN_SCALING, -32768 * DARAN_SCALING) / DARAN_SCALING;
        break;
    }
    }

    CAN_SendData(hfdcan, FDCAN_STANDARD_ID, ID << 5 | DARAN_CMD_POS_MODE, TxData, 8);
}

void DaRan_SetSpd(FDCAN_HandleTypeDef *hfdcan, unsigned char ID, unsigned char DARAN_SPD_MODE, float accel)
{
    unsigned char TxData[8];
    unsigned char ID_array = ID == DARAN_ID_BCAST ? DARAN_NUM : ID - DARAN_ID_OFFSET;

    *(float *)TxData = DaRan[ID_array].ctrl.spd / 60;
    *(short *)&TxData[4] = DARAN_SPD_MODE == DARAN_SPD_MODE_FDFWD ? LIMIT_RANGE(DaRan[ID_array].ctrl.trq, 32767 * DARAN_SCALING, -32768 * DARAN_SCALING) / DARAN_SCALING : LIMIT_RANGE(accel, 32767 * DARAN_SCALING, -32768 * DARAN_SCALING) / DARAN_SCALING;
    *(unsigned short *)&TxData[6] = DARAN_SPD_MODE;

    CAN_SendData(hfdcan, FDCAN_STANDARD_ID, ID << 5 | DARAN_CMD_SPD, TxData, 8);
}

void DaRan_SetTrq(FDCAN_HandleTypeDef *hfdcan, unsigned char ID, unsigned char DARAN_TRQ_MODE, float accel)
{
    unsigned char TxData[8];
    unsigned char ID_array = ID == DARAN_ID_BCAST ? DARAN_NUM : ID - DARAN_ID_OFFSET;

    *(float *)TxData = DaRan[ID_array].ctrl.trq;
    *(short *)&TxData[4] = DARAN_TRQ_MODE == DARAN_TRQ_MODE_DIRECT ? 0 : LIMIT_RANGE(accel, 32767 * DARAN_SCALING, -32768 * DARAN_SCALING) / DARAN_SCALING;
    *(unsigned short *)&TxData[6] = DARAN_TRQ_MODE;

    CAN_SendData(hfdcan, FDCAN_STANDARD_ID, ID << 5 | DARAN_CMD_TRQ, TxData, 8);
}

void DaRan_Prop_W(FDCAN_HandleTypeDef *hfdcan, unsigned char ID, unsigned short DARAN_PARAM, unsigned short DARAN_DATA_TYPE, float param_val)
{
    unsigned char TxData[8];

    *(unsigned short *)TxData = DARAN_PARAM;
    *(unsigned short *)&TxData[2] = DARAN_DATA_TYPE;
    switch (DARAN_DATA_TYPE)
    {
    case DARAN_DATA_TYPE_f:
    {
        *(float *)&TxData[4] = param_val;
        break;
    }
    case DARAN_DATA_TYPE_u16:
    {
        *(unsigned short *)&TxData[4] = param_val;
        break;
    }
    case DARAN_DATA_TYPE_s16:
    {
        *(short *)&TxData[4] = param_val;
        break;
    }
    case DARAN_DATA_TYPE_u32:
    {
        *(unsigned int *)&TxData[4] = param_val;
        break;
    }
    case DARAN_DATA_TYPE_s32:
    {
        *(int *)&TxData[4] = param_val;
        break;
    }
    }

    CAN_SendData(hfdcan, FDCAN_STANDARD_ID, ID << 5 | DARAN_CMD_PROP_W, TxData, 8);
}

void DaRan_Prop_R(FDCAN_HandleTypeDef *hfdcan, unsigned char ID, unsigned short DARAN_PARAM, unsigned short DARAN_DATA_TYPE)
{
    unsigned char TxData[8];

    *(unsigned short *)TxData = DARAN_PARAM;
    *(unsigned short *)&TxData[2] = DARAN_DATA_TYPE;
    *(unsigned int *)&TxData[4] = 0;

    CAN_SendData(hfdcan, FDCAN_STANDARD_ID, ID << 5 | DARAN_CMD_PROP_W, TxData, 8);
}

/*
void FDCAN1_IT0_IRQHandler(void)
{
    if (FDCAN1->IR & 0x1)
    {
        FDCAN1->IR |= 0x1;

        FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
        uint8_t RxData[64];
        HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxData);

        switch (FDCAN_RxHeader.Identifier)
        {
        case 1 << 5 | DARAN_CMD_PROP_R:
        {
            DaRan[1 - DARAN_ID_OFFSET].fdbk.pos = *(float *)RxData;
            DaRan[1 - DARAN_ID_OFFSET].fdbk.spd = *(short *)&RxData[4] / DARAN_fSPD;
            DaRan[1 - DARAN_ID_OFFSET].fdbk.trq = *(short *)&RxData[6] * DARAN_SCALING;
            break;
        }
        }
    }
}
*/
#endif