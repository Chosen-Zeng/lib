#ifndef __HAITAI_H
#define __HAITAI_H

#include "usr.h"

#ifdef HAITAI_NUM

#define HAITAI_SYS_INFO 0x0A
#define HAITAI_SYS_DATA 0x0B
#define HAITAI_SYS_PARAM_R 0x0C
#define HAITAI_SYS_PARAM_W 0x0D
#define HAITAI_SYS_PARAM_P 0x0E
#define HAITAI_SYS_PARAM_RST 0x0F
#define HAITAI_ENC_CAL 0x20
#define HAITAI_ENC_0POS 0x21
#define HAITAI_ENC_INFO 0x2F
#define HAITAI_STATUS_INFO 0x40
#define HAITAI_STATUS_CLEAR_ERR 0x41
#define HAITAI_CTRL_OFF 0x50
#define HAITAI_CTRL_0POS_MPL 0x51
#define HAITAI_CTRL_0POS_SGL 0x52
#define HAITAI_CTRL_PWR 0x53
#define HAITAI_CTRL_SPD 0x54
#define HAITAI_CTRL_POS_ABS 0x55
#define HAITAI_CTRL_POS_REL 0x56
#define HAITAI_CTRL_POS_SPD_CFG 0x57

#define HAITAI_fPOS (360.f / 16384)
#define HAITAI_fSPD (0.1f * 60)
#define HAITAI_fVOLT 0.2f
#define HAITAI_fCURR 0.03f
#define HAITAI_fTEMP 0.04f

#define HAITAI_PWR_MAX (1 << 15 - 1)
#define HAITAI_PWR_MIN (-1 << 15)
#define HAITAI_SPD_MAX ((1 << 15 - 1) * HAITAI_fSPD)
#define HAITAI_SPD_MIN ((-1 << 15) * HAITAI_fSPD)
#define HAITAI_POS_MAX (1 << 17 - 1)
#define HAITAI_POS_MIN (-1 << 17)

#define HAITAI_POS_SPD_LIM_R 0
#define HAITAI_POS_SPD_LIM_W 1

#define HAITAI_ERR_VOLT 0b001
#define HAITAI_ERR_CURR 0b010
#define HAITAI_ERR_TEMP 0b100

#define HAITAI_MODE_OFF 0 // fdbk status
#define HAITAI_MODE_PWR 1 // fdbk status
#define HAITAI_MODE_SPD 3 // fdbk status
#define HAITAI_MODE_POS 5 // fdbk status

#define HAITAI_PREAMBLE_SEND 0x3E
#define HAITAI_PREAMBLE_RECV 0x3C
#ifndef HAITAI_PID
#define HAITAI_PID 0 // PID 0 by default
#endif

typedef struct
{
    const unsigned char ID;

    struct
    {
        float pos, spd, pwr;
    } ctrl;
    struct
    {
        float pos_sgl, pos_mpl, spd;
    } fdbk;
    unsigned char TxData[11]; // for RS485
} HaiTai_t;
extern HaiTai_t HaiTai[HAITAI_NUM];

void HaiTai_SendCmd_CAN(CAN_handle_t *const CAN_handle, const unsigned char arrID, const unsigned char HAITAI_cmd);
void HaiTai_SendCmd_RS485(USART_handle_t *const UART_info, const unsigned char arrID, const unsigned char HAITAI_cmd);

bool HaiTai_MsgHandler_CAN(const unsigned CAN_ID, const unsigned char arrID, const unsigned char RxData[8]);
bool HaiTai_MsgHandler_RS485(const unsigned char arrID, const unsigned char RxData[15]);

#endif
#endif