#include "M2006.h"
#include "TIM.h"
#include "algorithm.h"
#include "CAN.h"

#ifdef FREQ_CTRL

C610_t C610[8];

C610_PID_t C610_PID_spd[8], C610_PID_pos[8];

void C610_SetCurrent(void *CAN_handle, unsigned short C610_ID)
{
    unsigned char TxData[16];

    for (unsigned char ID_array = (C610_ID == C610_ID2 ? 4 : 0); ID_array < (C610_ID == C610_ID1 ? 4 : 8); ID_array++)
    {
        TxData[ID_array * 2] = (unsigned short)(LIMIT_ABS(C610[ID_array].ctrl.curr, C610_CURR_LIMIT) / C610_fCURR) >> 8;
        TxData[ID_array * 2 + 1] = (unsigned short)(C610[ID_array].ctrl.curr / C610_fCURR);
    }

    if (C610_ID == (C610_ID1 | C610_ID2))
    {
#ifdef CAN_SUPPORT
        CAN_SendData(CAN_handle, CAN_ID_STD, C610_ID1, TxData, 8);
        CAN_SendData(CAN_handle, CAN_ID_STD, C610_ID2, &TxData[8], 8);
#elif defined FDCAN_SUPPORT
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C610_ID1, TxData, 8);
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C610_ID2, &TxData[8], 8);
#endif
    }
    else
#ifdef CAN_SUPPORT
        CAN_SendData(CAN_handle, CAN_ID_STD, C610_ID, C610_ID == C610_ID1 ? TxData : &TxData[8], 8);
#elif defined FDCAN_SUPPORT
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C610_ID, C610_ID == C610_ID1 ? TxData : &TxData[8], 8);
#endif
}

void C610_SetSpd(void *CAN_handle, unsigned short C610_ID)
{
    for (unsigned char ID_array = (C610_ID == C610_ID2 ? 4 : 0); ID_array < (C610_ID == C610_ID1 ? 4 : 8); ID_array++)
    {
        C610_PID_spd[ID_array].pterm = C610[ID_array].ctrl.spd - C610[ID_array].fdbk.spd;
        C610_PID_spd[ID_array].p = C610_PID_spd[ID_array].pterm * C610_SPD_Kp;

        if (ABS(C610_PID_spd[ID_array].pterm) >= C610_SPD_iSTART) // 积分分离
            C610_PID_spd[ID_array].iterm *= 0.4;
        else
            C610_PID_spd[ID_array].iterm += C610_PID_spd[ID_array].pterm / FREQ_CTRL;

        C610_PID_spd[ID_array].i = LIMIT_ABS(C610_PID_spd[ID_array].iterm, C610_SPD_iLIMIT) * C610_SPD_Ki; // 积分限幅

        C610_PID_spd[ID_array].dterm = (C610_PID_spd[ID_array].decurr - C610_PID_spd[ID_array].deprev) * FREQ_CTRL; // 微分先行
        C610_PID_spd[ID_array].d = C610_PID_spd[ID_array].dterm * C610_SPD_Kd;

        C610[ID_array].ctrl.curr = C610_PID_spd[ID_array].p + C610_PID_spd[ID_array].i + C610_PID_spd[ID_array].d;
    }
    C610_SetCurrent(CAN_handle, C610_ID);
}

void C610_SetPos(void *CAN_handle, unsigned short C610_ID)
{
    for (unsigned char ID_array = (C610_ID == C610_ID2 ? 4 : 0); ID_array < (C610_ID == C610_ID1 ? 4 : 8); ID_array++)
    {
        C610_PID_pos[ID_array].pterm = C610[ID_array].ctrl.pos - C610[ID_array].fdbk.pos;
        C610_PID_pos[ID_array].p = C610_PID_pos[ID_array].pterm * C610_POS_Kp;

        if (ABS(C610_PID_pos[ID_array].pterm) >= C610_POS_iSTART) // 积分分离
            C610_PID_pos[ID_array].iterm = 0;
        else if (ABS(C610_PID_pos[ID_array].iterm) <= C610_POS_iLIMIT)
            C610_PID_pos[ID_array].iterm += C610_PID_pos[ID_array].pterm / FREQ_CTRL;

        C610_PID_pos[ID_array].i = LIMIT_ABS(C610_PID_pos[ID_array].iterm, C610_POS_iLIMIT) * C610_POS_Ki; // 积分限幅

        C610_PID_pos[ID_array].dterm = (C610_PID_pos[ID_array].decurr - C610_PID_pos[ID_array].deprev) * FREQ_CTRL; // 微分先行
        C610_PID_pos[ID_array].d = C610_PID_pos[ID_array].dterm * C610_POS_Kd;

        C610[ID_array].ctrl.spd = C610_PID_pos[ID_array].p + C610_PID_pos[ID_array].i + C610_PID_pos[ID_array].d;
    }
    C610_SetSpd(CAN_handle, C610_ID);
}

void C610_SetTorque(void *CAN_handle, unsigned short C610_ID)
{
    for (unsigned char ID_array = (C610_ID == C610_ID2 ? 4 : 0); ID_array < (C610_ID == C610_ID1 ? 4 : 8); ID_array++)
        C610[ID_array].ctrl.curr = C610[ID_array].ctrl.trq / M2006_fTRQ;

    C610_SetCurrent(CAN_handle, C610_ID);
}

#ifdef CAN_SUPPORT
#elif defined FDCAN_SUPPORT
__weak void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, unsigned int RxFifo1ITs)
{
    FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
    unsigned char RxFifo1[8];
    if (hfdcan->Instance == FDCAN2)
    {
        // sw reset unusable
        /*
        static struct // add delay at initialization to set sw zero point
        {
            unsigned char ZP_Status[8];
            float ZP[8];
        } C610_ZP;
        */

        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &FDCAN_RxHeader, RxFifo1);
        unsigned char ID_array = FDCAN_RxHeader.Identifier - 0x200 - 1;

        static float pos_prev[8], pos_curr[8];
        /*
        if (!C610_ZP.ZP_Status)
        {
            pos_prev = pos_curr = C610_ZP.ZP = ((RxFifo1[0] << 8) | RxFifo1[1]) * C610_fPOS / C610_GR;
            C610_ZP.ZP_Status = 1;
        }
        else
        */
        {
            pos_curr[ID_array] = ((RxFifo1[0] << 8) | RxFifo1[1]) * C610_fPOS;
            if (ABS(pos_prev - pos_curr) >= 180) // 计圈
                C610[ID_array].fdbk.pos += ((pos_prev > pos_curr ? 360 : -360) + pos_curr - pos_prev) / M2006_GR;
            else
                C610[ID_array].fdbk.pos += (pos_curr - pos_prev) / M2006_GR;
            pos_prev[ID_array] = pos_curr[ID_array];
        }

        C610[ID_array].fdbk.spd = (unsigned short)((RxFifo1[2] << 8) | RxFifo1[3]) / M2006_GR;
        C610[ID_array].fdbk.curr = (unsigned short)((RxFifo1[4] << 8) | RxFifo1[5]) * C610_fCURR;
        C610[ID_array].fdbk.trq = C610[ID_array].fdbk.curr * M2006_fTRQ;

        C610_PID_spd[ID_array].deprev = C610_PID_spd[ID_array].decurr;
        C610_PID_spd[ID_array].decurr = C610[ID_array].ctrl.spd - C610[ID_array].fdbk.spd;

        C610_PID_pos[ID_array].deprev = C610_PID_pos[ID_array].decurr;
        C610_PID_pos[ID_array].decurr = C610[ID_array].ctrl.pos - C610[ID_array].fdbk.pos;
    }
}
#endif
#endif