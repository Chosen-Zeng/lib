#include "HaiTai.h"
#include "CAN.h"
#include "USART.h"

#if defined HAITAI_NUM && defined HAITAI_ID_OFFSET

HaiTai_t HaiTai[HAITAI_NUM];

void HaiTai_CAN_SendCmd(FDCAN_HandleTypeDef *hfdcan, unsigned char ID, unsigned char HAITAI_cmd)
{
    unsigned char TxData[4], len = 0;

    switch (HAITAI_cmd)
    {
    case HAITAI_CTRL_OFF:
    case HAITAI_CTRL_0POS_MPL:
    case HAITAI_CTRL_0POS_SGL:
        break;
    case HAITAI_CTRL_PWR:
    {
        len = 2;
        *(short *)TxData = LIMIT_RANGE(HaiTai[ID - HAITAI_ID_OFFSET].ctrl.pwr, HAITAI_PWR_MAX, HAITAI_PWR_MIN);
        break;
    }
    case HAITAI_CTRL_SPD:
    {
        len = 2;
        *(short *)TxData = LIMIT_RANGE(HaiTai[ID - HAITAI_ID_OFFSET].ctrl.spd, HAITAI_SPD_MAX, HAITAI_SPD_MIN) / HAITAI_fSPD;
        break;
    }
    case HAITAI_CTRL_POS_ABS:
    {
        len = 4;
        *(int *)TxData = LIMIT_RANGE(HaiTai[ID - HAITAI_ID_OFFSET].ctrl.pos, HAITAI_POS_MAX, HAITAI_POS_MIN) / HAITAI_fPOS;
        break;
    }
    case HAITAI_CTRL_POS_REL:
    {
        len = 4;
        *(int *)TxData = LIMIT_RANGE(HaiTai[ID - HAITAI_ID_OFFSET].ctrl.pos, HAITAI_POS_MAX, HAITAI_POS_MIN) / HAITAI_fPOS;
        break;
    }
    case HAITAI_CTRL_POS_SPD_CFG:
    {
        len = 3;
        TxData[0] = HAITAI_POS_SPD_LIM_W;
        *(short *)&TxData[1] = LIMIT_RANGE(HaiTai[ID - HAITAI_ID_OFFSET].ctrl.spd, HAITAI_SPD_MAX, HAITAI_SPD_MIN) / HAITAI_fSPD;
        break;
    }
    }

    CAN_SendData(hfdcan, FDCAN_STANDARD_ID, HAITAI_cmd << 4 | ID, TxData, len);
}

unsigned char HaiTai_TxData[11], HaiTai_RxData[33];
void HaiTai_RS485_SendCmd(USART_TypeDef *USART_handle, unsigned char ID, unsigned char HAITAI_cmd, DMA_TypeDef *DMA_handle, void *DMA_handle_sub, unsigned char DMA_sub_ID)
{
    HaiTai_TxData[0] = HAITAI_HEAD_SEND;
    HaiTai_TxData[1] = HAITAI_PID;
    HaiTai_TxData[2] = ID;
    HaiTai_TxData[3] = HAITAI_cmd;

    switch (HAITAI_cmd)
    {
    case HAITAI_SYS_PARAM_W:
    case HAITAI_SYS_PARAM_P:
        // undefined
        return;
    case HAITAI_CTRL_PWR:
    {
        HaiTai_TxData[4] = 2;
        *(short *)&HaiTai_TxData[5] = LIMIT_RANGE(HaiTai[ID - HAITAI_ID_OFFSET].ctrl.pwr, HAITAI_PWR_MAX, HAITAI_PWR_MIN);
        break;
    }
    case HAITAI_CTRL_SPD:
    {
        HaiTai_TxData[4] = 2;
        *(short *)&HaiTai_TxData[5] = LIMIT_RANGE(HaiTai[ID - HAITAI_ID_OFFSET].ctrl.spd, HAITAI_SPD_MAX, HAITAI_SPD_MIN) / HAITAI_fSPD;
        break;
    }
    case HAITAI_CTRL_POS_ABS:
    {
        HaiTai_TxData[4] = 4;
        *(int *)&HaiTai_TxData[5] = LIMIT_RANGE(HaiTai[ID - HAITAI_ID_OFFSET].ctrl.pos, HAITAI_POS_MAX, HAITAI_POS_MIN) / HAITAI_fPOS;
        break;
    }
    case HAITAI_CTRL_POS_REL:
    {
        HaiTai_TxData[4] = 4;
        *(int *)&HaiTai_TxData[5] = LIMIT_RANGE(HaiTai[ID - HAITAI_ID_OFFSET].ctrl.pos, HAITAI_POS_MAX, HAITAI_POS_MIN) / HAITAI_fPOS;
        break;
    }
    case HAITAI_CTRL_POS_SPD_CFG:
    {
        HaiTai_TxData[4] = 3;
        HaiTai_TxData[5] = HAITAI_POS_SPD_LIM_W;
        *(short *)&HaiTai_TxData[6] = LIMIT_RANGE(HaiTai[ID - HAITAI_ID_OFFSET].ctrl.spd, HAITAI_SPD_MAX, HAITAI_SPD_MIN) / HAITAI_fSPD;
        break;
    }
    }

    *(unsigned short *)&HaiTai_TxData[4 + HaiTai_TxData[4] + 1] = CRC_16_Cal(&CRC_16_Modbus, HaiTai_TxData, 5 + HaiTai_TxData[4]);

    UART_SendArray(USART_handle, HaiTai_TxData, 7 + HaiTai_TxData[4], DMA_handle, DMA_handle_sub, DMA_sub_ID);
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

        unsigned char ID_array = (FDCAN_RxHeader.Identifier & 0xF) - HAITAI_ID_OFFSET;

        HaiTai[ID_array].fdbk.pos_sgl = *(unsigned short *)RxData * HAITAI_fPOS;
        HaiTai[ID_array].fdbk.pos_mpl = *(int *)&RxData[2] * HAITAI_fPOS;
        HaiTai[ID_array].fdbk.spd = *(short *)&RxData[6] * HAITAI_fSPD;
    }
}
*/
#endif