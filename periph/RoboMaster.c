#include "RoboMaster.h"
#include "algorithm.h"
#include "CAN.h"

#ifdef FREQ_CTRL

C610_t C610[8];

PID_t C610_PID_spd[8], C610_PID_pos[8];
// default param:
// pos
// Kp 16
// Ki 12
// Kd 1
// i_start 60
// i_lim 0.8f
// spd
// Kp 0.4f
// Ki 0.25f
// Kd 2.7e-3f
// i_start 100
// i_lim 5

void C610_SetCurr(CAN_handle_t *const CAN_handle, const unsigned short C610_ID)
{
    unsigned char TxData[C610_ID == (C610_ID1 | C610_ID2) ? 16 : 8];

    for (unsigned char arrID = (C610_ID == C610_ID2 ? 4 : 0), offset = arrID; arrID < (C610_ID == C610_ID1 ? 4 : 8); ++arrID)
    {
        TxData[(arrID - offset) * 2] = (short)(LIMIT_ABS(C610[arrID].ctrl.curr, C610_CURR_LIMIT) / C610_fCURR) >> 8;
        TxData[(arrID - offset) * 2 + 1] = (short)(C610[arrID].ctrl.curr / C610_fCURR);
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
        CAN_SendData(CAN_handle, CAN_ID_STD, C610_ID, TxData, 8);
#elif defined FDCAN_SUPPORT
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C610_ID, TxData, 8);
#endif
}

void C610_SetSpd(CAN_handle_t *const CAN_handle, const unsigned short C610_ID)
{
    static float iterm[8];

    for (unsigned char arrID = (C610_ID == C610_ID2 ? 4 : 0); arrID < (C610_ID == C610_ID1 ? 4 : 8); ++arrID)
    {
        C610_PID_spd[arrID].p = (C610[arrID].ctrl.spd - C610[arrID].fdbk.spd) * C610_PID_spd[arrID].Kp;

        if (ABS(C610[arrID].ctrl.spd - C610[arrID].fdbk.spd) >= C610_PID_spd[arrID].i_start) // integral separation
            iterm[arrID] *= 0.4f;
        else
            iterm[arrID] += (C610[arrID].ctrl.spd - C610[arrID].fdbk.spd) / FREQ_CTRL;

        C610_PID_spd[arrID].i = LIMIT_ABS(iterm[arrID], C610_PID_spd[arrID].i_lim) * C610_PID_spd[arrID].Ki; // integral limit

        C610_PID_spd[arrID].d = (C610_PID_spd[arrID].decurr - C610_PID_spd[arrID].deprev) * FREQ_CTRL * C610_PID_spd[arrID].Kd; // derivative filtering

        C610[arrID].ctrl.curr = C610_PID_spd[arrID].p + C610_PID_spd[arrID].i + C610_PID_spd[arrID].d;
    }
    C610_SetCurr(CAN_handle, C610_ID);
}

void C610_SetPos(CAN_handle_t *const CAN_handle, const unsigned short C610_ID)
{
    static float iterm[8];

    for (unsigned char arrID = (C610_ID == C610_ID2 ? 4 : 0); arrID < (C610_ID == C610_ID1 ? 4 : 8); ++arrID)
    {
        C610_PID_pos[arrID].p = (C610[arrID].ctrl.pos - C610[arrID].fdbk.pos) * C610_PID_pos[arrID].Kp;

        if (ABS(C610[arrID].ctrl.pos - C610[arrID].fdbk.pos) >= C610_PID_pos[arrID].i_start) // integral separation
            iterm[arrID] = 0;
        else
            iterm[arrID] += (C610[arrID].ctrl.pos - C610[arrID].fdbk.pos) / FREQ_CTRL;

        C610_PID_pos[arrID].i = LIMIT_ABS(iterm[arrID], C610_PID_pos[arrID].i_lim) * C610_PID_pos[arrID].Ki; // integral limit

        C610_PID_pos[arrID].d = (C610_PID_pos[arrID].decurr - C610_PID_pos[arrID].deprev) * FREQ_CTRL * C610_PID_pos[arrID].Kd; // derivative filtering

        C610[arrID].ctrl.spd = C610_PID_pos[arrID].p + C610_PID_pos[arrID].i + C610_PID_pos[arrID].d;
    }
    C610_SetSpd(CAN_handle, C610_ID);
}

void C610_SetTrq(CAN_handle_t *const CAN_handle, const unsigned short C610_ID)
{
    for (unsigned char arrID = (C610_ID == C610_ID2 ? 4 : 0); arrID < (C610_ID == C610_ID1 ? 4 : 8); ++arrID)
        C610[arrID].ctrl.curr = C610[arrID].ctrl.trq / M2006_fTRQ;

    C610_SetCurr(CAN_handle, C610_ID);
}

bool C610_MsgHandler(unsigned CAN_ID, unsigned char RxData[8])
{
    if (CAN_ID >= 0x201 && CAN_ID <= 0x208)
    {
        static float pos_prev[8], pos_curr[8];
        unsigned char arrID = CAN_ID - 0x201;

        pos_curr[arrID] = ((RxData[0] << 8) | RxData[1]) * C610_fPOS;
        if (ABS(pos_prev[arrID] - pos_curr[arrID]) >= 180) // lap
            C610[arrID].fdbk.pos += ((pos_prev[arrID] > pos_curr[arrID] ? 360 : -360) + pos_curr[arrID] - pos_prev[arrID]) / M2006_GR;
        else
            C610[arrID].fdbk.pos += (pos_curr[arrID] - pos_prev[arrID]) / M2006_GR;
        pos_prev[arrID] = pos_curr[arrID];

        C610[arrID].fdbk.spd = (short)((RxData[2] << 8) | RxData[3]) / M2006_GR;
        C610[arrID].fdbk.curr = (short)((RxData[4] << 8) | RxData[5]) * C610_fCURR;
        C610[arrID].fdbk.trq = C610[arrID].fdbk.curr * M2006_fTRQ;

        C610_PID_spd[arrID].deprev = C610_PID_spd[arrID].decurr;
        C610_PID_spd[arrID].decurr = C610[arrID].ctrl.spd - C610[arrID].fdbk.spd;

        C610_PID_pos[arrID].deprev = C610_PID_pos[arrID].decurr;
        C610_PID_pos[arrID].decurr = C610[arrID].ctrl.pos - C610[arrID].fdbk.pos;

        return true;
    }
    return false;
}

C620_t C620[8];

PID_t C620_PID_spd[8], C620_PID_pos[8];
// default param:
// pos
// Kp 19
// Ki 3
// Kd 0.35f
// i_start 2
// i_lim 2
// spd
// Kp 0.4f
// Ki 0.6f
// Kd 1.5e-3f
// i_start 50
// i_lim 10

void C620_SetCurr(CAN_handle_t *const CAN_handle, const unsigned short C620_ID)
{
    unsigned char TxData[C620_ID == (C620_ID1 | C620_ID2) ? 16 : 8];

    for (unsigned char arrID = (C620_ID == C620_ID2 ? 4 : 0), offset = arrID; arrID < (C620_ID == C620_ID1 ? 4 : 8); ++arrID)
    {
        TxData[(arrID - offset) * 2] = (short)(LIMIT_ABS(C620[arrID].ctrl.curr, C620_CURR_LIMIT) / C620_fCURR) >> 8;
        TxData[(arrID - offset) * 2 + 1] = (short)(C620[arrID].ctrl.curr / C620_fCURR);
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
        CAN_SendData(CAN_handle, CAN_ID_STD, C620_ID, TxData, 8);
#elif defined FDCAN_SUPPORT
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C620_ID, TxData, 8);
#endif
}

void C620_SetSpd(CAN_handle_t *const CAN_handle, const unsigned short C620_ID)
{
    static float iterm[8];

    for (unsigned char arrID = (C620_ID == C620_ID2 ? 4 : 0); arrID < (C620_ID == C620_ID1 ? 4 : 8); ++arrID)
    {
        C620_PID_spd[arrID].p = (C620[arrID].ctrl.spd - C620[arrID].fdbk.spd) * C620_PID_spd[arrID].Kp;

        if (ABS(C620[arrID].ctrl.spd - C620[arrID].fdbk.spd) >= C620_PID_spd[arrID].i_start) // integral separation
            iterm[arrID] *= 0.4;
        else
            iterm[arrID] += (C620[arrID].ctrl.spd - C620[arrID].fdbk.spd) / FREQ_CTRL;

        C620_PID_spd[arrID].i = LIMIT_ABS(iterm[arrID], C620_PID_spd[arrID].i_lim) * C620_PID_spd[arrID].Ki; // integral limit

        C620_PID_spd[arrID].d = (C620_PID_spd[arrID].decurr - C620_PID_spd[arrID].deprev) * FREQ_CTRL * C620_PID_spd[arrID].Kd; // derivative filtering

        C620[arrID].ctrl.curr = C620_PID_spd[arrID].p + C620_PID_spd[arrID].i + C620_PID_spd[arrID].d;
    }
    C620_SetCurr(CAN_handle, C620_ID);
}

void C620_SetPos(CAN_handle_t *const CAN_handle, const unsigned short C620_ID)
{
    static float iterm[8];

    for (unsigned char arrID = (C620_ID == C620_ID2 ? 4 : 0); arrID < (C620_ID == C620_ID1 ? 4 : 8); ++arrID)
    {
        C620_PID_pos[arrID].p = (C620[arrID].ctrl.pos - C620[arrID].fdbk.pos) * C620_PID_pos[arrID].Kp;

        if (ABS(C620[arrID].ctrl.pos - C620[arrID].fdbk.pos) >= C620_PID_pos[arrID].i_start) // integral separation
            iterm[arrID] = 0;
        else
            iterm[arrID] += (C620[arrID].ctrl.pos - C620[arrID].fdbk.pos) / FREQ_CTRL;

        C620_PID_pos[arrID].i = LIMIT_ABS(iterm[arrID], C620_PID_pos[arrID].i_lim) * C620_PID_pos[arrID].Ki; // integral limit

        C620_PID_pos[arrID].d = (C620_PID_pos[arrID].decurr - C620_PID_pos[arrID].deprev) * FREQ_CTRL * C620_PID_pos[arrID].Kd; // derivative filtering

        C620[arrID].ctrl.spd = C620_PID_pos[arrID].p + C620_PID_pos[arrID].i + C620_PID_pos[arrID].d;
    }
    C620_SetSpd(CAN_handle, C620_ID);
}

void C620_SetTrq(CAN_handle_t *const CAN_handle, const unsigned short C620_ID)
{
    for (unsigned char arrID = (C620_ID == C620_ID2 ? 4 : 0); arrID < (C620_ID == C620_ID1 ? 4 : 8); ++arrID)
        C620[arrID].ctrl.curr = C620[arrID].ctrl.trq / M3508_fTRQ;

    C620_SetCurr(CAN_handle, C620_ID);
}

bool C620_MsgHandler(unsigned CAN_ID, unsigned char RxData[8])
{
    if (CAN_ID >= 0x201 && CAN_ID <= 0x208)
    {
        static float pos_prev[8], pos_curr[8];
        unsigned char arrID = CAN_ID - 0x201;

        pos_curr[arrID] = ((RxData[0] << 8) | RxData[1]) * C620_fPOS;
        if (ABS(pos_prev[arrID] - pos_curr[arrID]) >= 180) // lap count
            C620[arrID].fdbk.pos += ((pos_prev[arrID] > pos_curr[arrID] ? 360 : -360) - pos_prev[arrID] + pos_curr[arrID]) / M3508_GR;
        else
            C620[arrID].fdbk.pos += (pos_curr[arrID] - pos_prev[arrID]) / M3508_GR;
        pos_prev[arrID] = pos_curr[arrID];

        C620[arrID].fdbk.spd = (short)((RxData[2] << 8) | RxData[3]) / M3508_GR;
        C620[arrID].fdbk.curr = (short)((RxData[4] << 8) | RxData[5]) * C620_fCURR;
        C620[arrID].fdbk.trq = C620[arrID].fdbk.curr * M3508_fTRQ;
        C620[arrID].fdbk.temp = RxData[6];

        C620_PID_spd[arrID].deprev = C620_PID_spd[arrID].decurr;
        C620_PID_spd[arrID].decurr = C620[arrID].ctrl.spd - C620[arrID].fdbk.spd;

        C620_PID_pos[arrID].deprev = C620_PID_pos[arrID].decurr;
        C620_PID_pos[arrID].decurr = C620[arrID].ctrl.pos - C620[arrID].fdbk.pos;

        return true;
    }
    return false;
}

#else
#error FREQ_CTRL undefined
#endif

#endif