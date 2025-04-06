#include "RoboMaster.h"
#include "algorithm.h"
#include "CAN.h"

#ifdef FREQ_CTRL

C610_t C610[8];
C620_t C620[8];

PID_t C610_PID_spd[8], C610_PID_pos[8], C620_PID_spd[8], C620_PID_pos[8];

void C610_SetCurr(void *CAN_handle, unsigned short C610_ID)
{
    unsigned char TxData[16];

    for (unsigned char arrID = (C610_ID == C610_ID2 ? 4 : 0); arrID < (C610_ID == C610_ID1 ? 4 : 8); arrID++)
    {
        TxData[arrID * 2] = (short)(LIMIT_ABS(C610[arrID].ctrl.curr, C610_CURR_LIMIT) / C610_fCURR) >> 8;
        TxData[arrID * 2 + 1] = (short)(C610[arrID].ctrl.curr / C610_fCURR);
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
    static float iterm[8];

    for (unsigned char arrID = (C610_ID == C610_ID2 ? 4 : 0); arrID < (C610_ID == C610_ID1 ? 4 : 8); arrID++)
    {
        C610_PID_spd[arrID].p = (C610[arrID].ctrl.spd - C610[arrID].fdbk.spd) * C610_SPD_Kp;

        if (ABS(C610[arrID].ctrl.spd - C610[arrID].fdbk.spd) >= C610_SPD_iSTART) // integral separation
            iterm[arrID] *= 0.4f;
        else
            iterm[arrID] += (C610[arrID].ctrl.spd - C610[arrID].fdbk.spd) / FREQ_CTRL;

        C610_PID_spd[arrID].i = LIMIT_ABS(iterm[arrID], C610_SPD_iLIMIT) * C610_SPD_Ki; // integral limit

        C610_PID_spd[arrID].d = (C610_PID_spd[arrID].decurr - C610_PID_spd[arrID].deprev) * FREQ_CTRL * C610_SPD_Kd; // derivative filtering

        C610[arrID].ctrl.curr = C610_PID_spd[arrID].p + C610_PID_spd[arrID].i + C610_PID_spd[arrID].d;
    }
    C610_SetCurr(CAN_handle, C610_ID);
}

void C610_SetPos(void *CAN_handle, unsigned short C610_ID)
{
    static float iterm[8];

    for (unsigned char arrID = (C610_ID == C610_ID2 ? 4 : 0); arrID < (C610_ID == C610_ID1 ? 4 : 8); arrID++)
    {
        C610_PID_pos[arrID].p = (C610[arrID].ctrl.pos - C610[arrID].fdbk.pos) * C610_POS_Kp;

        if (ABS(C610[arrID].ctrl.pos - C610[arrID].fdbk.pos) >= C610_POS_iSTART) // integral separation
            iterm[arrID] *= 0;
        else if (ABS(iterm[arrID]) <= C610_POS_iLIMIT)
            iterm[arrID] += (C610[arrID].ctrl.pos - C610[arrID].fdbk.pos) / FREQ_CTRL;

        C610_PID_pos[arrID].i = LIMIT_ABS(iterm[arrID], C610_POS_iLIMIT) * C610_POS_Ki; // integral limit

        C610_PID_pos[arrID].d = (C610_PID_pos[arrID].decurr - C610_PID_pos[arrID].deprev) * FREQ_CTRL * C610_POS_Kd; // derivative filtering

        C610[arrID].ctrl.spd = C610_PID_pos[arrID].p + C610_PID_pos[arrID].i + C610_PID_pos[arrID].d;
    }
    C610_SetSpd(CAN_handle, C610_ID);
}

void C610_SetTrq(void *CAN_handle, unsigned short C610_ID)
{
    for (unsigned char arrID = (C610_ID == C610_ID2 ? 4 : 0); arrID < (C610_ID == C610_ID1 ? 4 : 8); arrID++)
        C610[arrID].ctrl.curr = C610[arrID].ctrl.trq / M2006_fTRQ;

    C610_SetCurr(CAN_handle, C610_ID);
}

void C620_SetCurr(void *CAN_handle, unsigned short C620_ID)
{
    unsigned char TxData[16];

    for (unsigned char arrID = (C620_ID == C620_ID2 ? 4 : 0); arrID < (C620_ID == C620_ID1 ? 4 : 8); arrID++)
    {
        TxData[arrID * 2] = (short)(LIMIT_ABS(C620[arrID].ctrl.curr, C620_CURR_LIMIT) / C620_fCURR) >> 8;
        TxData[arrID * 2 + 1] = (short)(C620[arrID].ctrl.curr / C620_fCURR);
    }

    if (C620_ID == (C620_ID1 | C620_ID2))
    {
#ifdef CAN_SUPPORT
        CAN_SendData(CAN_handle, CAN_ID_STD, C620_ID1, TxData, 8);
        CAN_SendData(CAN_handle, CAN_ID_STD, C620_ID2, &TxData[8], 8);
#elif defined FDCAN_SUPPORT
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C620_ID1, TxData, 8);
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C620_ID2, &TxData[8], 8);
#endif
    }
    else
#ifdef CAN_SUPPORT
        CAN_SendData(CAN_handle, CAN_ID_STD, C620_ID, C620_ID == C620_ID1 ? TxData : &TxData[8], 8);
#elif defined FDCAN_SUPPORT
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C620_ID, C620_ID == C620_ID1 ? TxData : &TxData[8], 8);
#endif
}

void C620_SetSpd(void *CAN_handle, unsigned short C620_ID)
{
    static float iterm[8];

    for (unsigned char arrID = (C620_ID == C620_ID2 ? 4 : 0); arrID < (C620_ID == C620_ID1 ? 4 : 8); arrID++)
    {
        C620_PID_spd[arrID].p = (C620[arrID].ctrl.spd - C620[arrID].fdbk.spd) * C620_SPD_Kp;

        if (ABS(C620[arrID].ctrl.spd - C620[arrID].fdbk.spd) >= C620_SPD_iSTART) // integral separation
            iterm[arrID] *= 0.4;
        else
            iterm[arrID] += (C620[arrID].ctrl.spd - C620[arrID].fdbk.spd) / FREQ_CTRL;

        C620_PID_spd[arrID].i = LIMIT_ABS(iterm[arrID], C620_SPD_iLIMIT) * C620_SPD_Ki; // integral limit

        C620_PID_spd[arrID].d = (C620_PID_spd[arrID].decurr - C620_PID_spd[arrID].deprev) * FREQ_CTRL * C620_SPD_Kd; // derivative filtering

        C620[arrID].ctrl.curr = C620_PID_spd[arrID].p + C620_PID_spd[arrID].i + C620_PID_spd[arrID].d;
    }
    C620_SetCurr(CAN_handle, C620_ID);
}

void C620_SetPos(void *CAN_handle, unsigned short C620_ID)
{
    static float iterm[8];

    for (unsigned char arrID = (C620_ID == C620_ID2 ? 4 : 0); arrID < (C620_ID == C620_ID1 ? 4 : 8); arrID++)
    {
        C620_PID_pos[arrID].p = (C620[arrID].ctrl.pos - C620[arrID].fdbk.pos) * C620_POS_Kp;

        if (ABS(C620[arrID].ctrl.pos - C620[arrID].fdbk.pos) >= C620_POS_iSTART) // integral separation
            iterm[arrID] = 0;
        else if (ABS(iterm[arrID]) <= C620_POS_iLIMIT)
            iterm[arrID] += (C620[arrID].ctrl.pos - C620[arrID].fdbk.pos) / FREQ_CTRL;

        C620_PID_pos[arrID].i = LIMIT_ABS(iterm[arrID], C620_POS_iLIMIT) * C620_POS_Ki; // integral limit

        C620_PID_pos[arrID].d = (C620_PID_pos[arrID].decurr - C620_PID_pos[arrID].deprev) * FREQ_CTRL * C620_POS_Kd; // derivative filtering

        C620[arrID].ctrl.spd = C620_PID_pos[arrID].p + C620_PID_pos[arrID].i + C620_PID_pos[arrID].d;
    }
    C620_SetSpd(CAN_handle, C620_ID);
}

void C620_SetTrq(void *CAN_handle, unsigned short C620_ID)
{
    for (unsigned char arrID = (C620_ID == C620_ID2 ? 4 : 0); arrID < (C620_ID == C620_ID1 ? 4 : 8); arrID++)
        C620[arrID].ctrl.curr = C620[arrID].ctrl.trq / M3508_fTRQ;

    C620_SetCurr(CAN_handle, C620_ID);
}

/*void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, unsigned int RxFifo1ITs)
{
    FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
    unsigned char RxFifo1[8];
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &FDCAN_RxHeader, RxFifo1);
    unsigned char arrID = FDCAN_RxHeader.Identifier - 0x200 - 1;

    static float pos_prev, pos_curr;

    pos_curr = ((RxData[0] << 8) | RxData[1]) * C610_fPOS;
    if (ABS(pos_prev - pos_curr) >= 180) // lap count
        C610[0].fdbk.pos += ((pos_prev > pos_curr ? 360 : -360) + pos_curr - pos_prev) / M2006_GR;
    else
        C610[0].fdbk.pos += (pos_curr - pos_prev) / M2006_GR;
    pos_prev = pos_curr;

    C610[0].fdbk.spd = (short)((RxData[2] << 8) | RxData[3]) / M2006_GR;
    C610[0].fdbk.curr = (short)((RxData[4] << 8) | RxData[5]) * C610_fCURR;
    C610[0].fdbk.trq = C610[0].fdbk.curr * M2006_fTRQ;

    C610_PID_spd[0].deprev = C610_PID_spd[0].decurr;
    C610_PID_spd[0].decurr = C610[0].ctrl.spd - C610[0].fdbk.spd;

    C610_PID_pos[0].deprev = C610_PID_pos[0].decurr;
    C610_PID_pos[0].decurr = C610[0].ctrl.pos - C610[0].fdbk.pos;
}

__weak void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef CAN_RxHeader;
    unsigned char RxFifo0[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_RxHeader, RxFifo0);

    if (hcan->Instance == CAN1)
    {
        unsigned char arrID = CAN_RxHeader.StdId - 0x200 - 1;
        static float pos_prev[8], pos_curr[8];

        pos_curr[arrID] = ((RxFifo0[0] << 8) | RxFifo0[1]) * C620_fPOS;
        if (ABS(pos_prev[arrID] - pos_curr[arrID]) >= 180) // lap
            C620[arrID].fdbk.pos += ((pos_prev[arrID] > pos_curr[arrID] ? 360 : -360) - pos_prev[arrID] + pos_curr[arrID]) / M3508_GR;
        else
            C620[arrID].fdbk.pos += (pos_curr[arrID] - pos_prev[arrID]) / M3508_GR;
        pos_prev[arrID] = pos_curr[arrID];

        C620[arrID].fdbk.spd = (short)((RxFifo0[2] << 8) | RxFifo0[3]) / M3508_GR;
        C620[arrID].fdbk.curr = (short)((RxFifo0[4] << 8) | RxFifo0[5]) * C620_fCURR;
        C620[arrID].fdbk.trq = C620[arrID].fdbk.curr * M3508_fTRQ;
        C620[arrID].fdbk.temp = RxFifo0[6];

        C620_PID_spd[arrID].deprev = C620_PID_spd[arrID].decurr;
        C620_PID_spd[arrID].decurr = C620[arrID].ctrl.spd - C620[arrID].fdbk.spd;

        C620_PID_pos[arrID].deprev = C620_PID_pos[arrID].decurr;
        C620_PID_pos[arrID].decurr = C620[arrID].ctrl.pos - C620[arrID].fdbk.pos;
    }
}
 */
#endif