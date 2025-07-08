#include "HaiTai.h"
#include "algorithm.h"
#include "CAN.h"
#include "USART.h"
#include "CRC.h"

#ifdef HAITAI_NUM

void HaiTai_SendCmd_CAN(CAN_handle_t *const CAN_handle, const unsigned char arrID, const unsigned char HAITAI_cmd)
{
    unsigned char len = 0;

    switch (HAITAI_cmd)
    {
    case HAITAI_CTRL_OFF:
    case HAITAI_CTRL_0POS_MPL:
    case HAITAI_CTRL_0POS_SGL:
        break;
    case HAITAI_CTRL_PWR:
    {
        len = 2;
        *(short *)HaiTai[arrID].TxData = LIMIT_RANGE(HaiTai[arrID].ctrl.pwr, HAITAI_PWR_MAX, HAITAI_PWR_MIN);
        break;
    }
    case HAITAI_CTRL_SPD:
    {
        len = 2;
        *(short *)HaiTai[arrID].TxData = LIMIT_RANGE(HaiTai[arrID].ctrl.spd, HAITAI_SPD_MAX, HAITAI_SPD_MIN) / HAITAI_fSPD;
        break;
    }
    case HAITAI_CTRL_POS_ABS:
    {
        len = 4;
        *(int *)HaiTai[arrID].TxData = LIMIT_RANGE(HaiTai[arrID].ctrl.pos, HAITAI_POS_MAX, HAITAI_POS_MIN) / HAITAI_fPOS;
        break;
    }
    case HAITAI_CTRL_POS_REL:
    {
        len = 4;
        *(int *)HaiTai[arrID].TxData = LIMIT_RANGE(HaiTai[arrID].ctrl.pos, HAITAI_POS_MAX, HAITAI_POS_MIN) / HAITAI_fPOS;
        break;
    }
    case HAITAI_CTRL_POS_SPD_CFG:
    {
        len = 3;
        HaiTai[arrID].TxData[0] = HAITAI_POS_SPD_LIM_W;
        *(short *)&HaiTai[arrID].TxData[1] = LIMIT_RANGE(HaiTai[arrID].ctrl.spd, HAITAI_SPD_MAX, HAITAI_SPD_MIN) / HAITAI_fSPD;
        break;
    }
    default:
        return;
    }

#ifdef CAN_SUPPORT
    CAN_SendData(CAN_handle, CAN_ID_STD, HAITAI_cmd << 4 | HaiTai[arrID].ID, HaiTai[arrID].TxData, len);
#elif defined FDCAN_SUPPORT
    CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, HAITAI_cmd << 4 | HaiTai[arrID].ID, HaiTai[arrID].TxData, len);
#endif
}

void HaiTai_SendCmd_RS485(USART_handle_t *const UART_info, const unsigned char arrID, const unsigned char HAITAI_cmd)
{
    HaiTai[arrID].TxData[0] = HAITAI_PREAMBLE_SEND;
    HaiTai[arrID].TxData[1] = HAITAI_PID;
    HaiTai[arrID].TxData[2] = HaiTai[arrID].ID;
    HaiTai[arrID].TxData[3] = HAITAI_cmd;

    switch (HAITAI_cmd)
    {
    case HAITAI_SYS_PARAM_W:
    case HAITAI_SYS_PARAM_P:
    {
        // unknown behavior
        return;
    }
    case HAITAI_CTRL_PWR:
    {
        HaiTai[arrID].TxData[4] = 2;
        *(short *)&HaiTai[arrID].TxData[5] = LIMIT_RANGE(HaiTai[arrID].ctrl.pwr, HAITAI_PWR_MAX, HAITAI_PWR_MIN);
        break;
    }
    case HAITAI_CTRL_SPD:
    {
        HaiTai[arrID].TxData[4] = 2;
        *(short *)&HaiTai[arrID].TxData[5] = LIMIT_RANGE(HaiTai[arrID].ctrl.spd, HAITAI_SPD_MAX, HAITAI_SPD_MIN) / HAITAI_fSPD;
        break;
    }
    case HAITAI_CTRL_POS_ABS:
    {
        HaiTai[arrID].TxData[4] = 4;
        *(int *)&HaiTai[arrID].TxData[5] = LIMIT_RANGE(HaiTai[arrID].ctrl.pos, HAITAI_POS_MAX, HAITAI_POS_MIN) / HAITAI_fPOS;
        break;
    }
    case HAITAI_CTRL_POS_REL:
    {
        HaiTai[arrID].TxData[4] = 4;
        *(int *)&HaiTai[arrID].TxData[5] = LIMIT_RANGE(HaiTai[arrID].ctrl.pos, HAITAI_POS_MAX, HAITAI_POS_MIN) / HAITAI_fPOS;
        break;
    }
    case HAITAI_CTRL_POS_SPD_CFG:
    {
        HaiTai[arrID].TxData[4] = 3;
        HaiTai[arrID].TxData[5] = HAITAI_POS_SPD_LIM_W;
        *(short *)&HaiTai[arrID].TxData[6] = LIMIT_RANGE(HaiTai[arrID].ctrl.spd, HAITAI_SPD_MAX, HAITAI_SPD_MIN) / HAITAI_fSPD;
        break;
    }
    }

    *(unsigned short *)&HaiTai[arrID].TxData[5 + HaiTai[arrID].TxData[4]] = CRCsw_Calc(&CRC_16_MODBUS, HaiTai[arrID].TxData, 5 + HaiTai[arrID].TxData[4]);

    UART_SendArray(UART_info, HaiTai[arrID].TxData, 7 + HaiTai[arrID].TxData[4]);
}

bool HaiTai_MsgHandler_CAN(const unsigned CAN_ID, const unsigned char arrID, const unsigned char RxData[8])
{
    if (CAN_ID == (HAITAI_ENC_INFO << 4 | HaiTai[arrID].ID))
    {
        HaiTai[arrID].fdbk.pos_sgl = *(unsigned short *)RxData * HAITAI_fPOS;
        HaiTai[arrID].fdbk.pos_mpl = *(int *)&RxData[2] * HAITAI_fPOS;
        HaiTai[arrID].fdbk.spd = *(short *)&RxData[6] * HAITAI_fSPD;

        return true;
    }
    return false;
}

bool HaiTai_MsgHandler_RS485(const unsigned char arrID, const unsigned char RxData[15])
{
    if (HAITAI_PREAMBLE_RECV == RxData[0] &&
        PID == RxData[1] &&
        HaiTai[arrID].ID == RxData[2] &&
        HAITAI_ENC_INFO == RxData[3] &&
        8 == RxData[4]) // data length
    {
        HaiTai[arrID].fdbk.pos_sgl = *(unsigned short *)RxData * HAITAI_fPOS;
        HaiTai[arrID].fdbk.pos_mpl = *(int *)&RxData[2] * HAITAI_fPOS;
        HaiTai[arrID].fdbk.spd = *(short *)&RxData[6] * HAITAI_fSPD;

        return true;
    }
    return false;
}

#else
#error HAITAI_NUM undefined
#endif