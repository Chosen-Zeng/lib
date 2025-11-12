#include "RoboMaster.h"
#include "CAN.h"
#include "algorithm.h"

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

void C610_SetCurr(CAN_handle_t *const CAN_handle, const unsigned short C610_ID) {
    unsigned char TxData[C610_ID == (C610_ID1 | C610_ID2) ? 16 : 8];

    for (unsigned char idx = (C610_ID == C610_ID2 ? 4 : 0), offset = idx; idx < (C610_ID == C610_ID1 ? 4 : 8); ++idx) {
        TxData[(idx - offset) * 2] = (short)(LIMIT_ABS(C610[idx].ctrl.curr, C610_CURR_LIMIT) / C610_fCURR) >> 8;
        TxData[(idx - offset) * 2 + 1] = (short)(C610[idx].ctrl.curr / C610_fCURR);
    }

    if (C610_ID == (C610_ID1 | C610_ID2)) {
#ifdef CAN_SUPPORT
        CAN_SendData(CAN_handle, CAN_ID_STD, C610_ID1, TxData, 8);
        CAN_SendData(CAN_handle, CAN_ID_STD, C610_ID2, &TxData[8], 8);
#elif defined FDCAN_SUPPORT
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C610_ID1, TxData, 8);
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C610_ID2, &TxData[8], 8);
#endif
    } else
#ifdef CAN_SUPPORT
        CAN_SendData(CAN_handle, CAN_ID_STD, C610_ID, TxData, 8);
#elif defined FDCAN_SUPPORT
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C610_ID, TxData, 8);
#endif
}

void C610_SetSpd(CAN_handle_t *const CAN_handle, const unsigned short C610_ID) {
    static float iterm[8];

    for (unsigned char idx = (C610_ID == C610_ID2 ? 4 : 0); idx < (C610_ID == C610_ID1 ? 4 : 8); ++idx) {
        C610_PID_spd[idx].p = (C610[idx].ctrl.spd - C610[idx].fdbk.spd) * C610_PID_spd[idx].Kp;

        if (ABS(C610[idx].ctrl.spd - C610[idx].fdbk.spd) >= C610_PID_spd[idx].i_start) // integral separation
            iterm[idx] *= 0.4f;
        else
            iterm[idx] += (C610[idx].ctrl.spd - C610[idx].fdbk.spd) / FREQ_CTRL;

        C610_PID_spd[idx].i = LIMIT_ABS(iterm[idx], C610_PID_spd[idx].i_lim) * C610_PID_spd[idx].Ki; // integral limit

        C610_PID_spd[idx].d = (C610_PID_spd[idx].decurr - C610_PID_spd[idx].deprev) * FREQ_CTRL * C610_PID_spd[idx].Kd; // derivative filtering

        C610[idx].ctrl.curr = C610_PID_spd[idx].p + C610_PID_spd[idx].i + C610_PID_spd[idx].d;
    }
    C610_SetCurr(CAN_handle, C610_ID);
}

void C610_SetPos(CAN_handle_t *const CAN_handle, const unsigned short C610_ID) {
    static float iterm[8];

    for (unsigned char idx = (C610_ID == C610_ID2 ? 4 : 0); idx < (C610_ID == C610_ID1 ? 4 : 8); ++idx) {
        C610_PID_pos[idx].p = (C610[idx].ctrl.pos - C610[idx].fdbk.pos) * C610_PID_pos[idx].Kp;

        if (ABS(C610[idx].ctrl.pos - C610[idx].fdbk.pos) >= C610_PID_pos[idx].i_start) // integral separation
            iterm[idx] = 0;
        else
            iterm[idx] += (C610[idx].ctrl.pos - C610[idx].fdbk.pos) / FREQ_CTRL;

        C610_PID_pos[idx].i = LIMIT_ABS(iterm[idx], C610_PID_pos[idx].i_lim) * C610_PID_pos[idx].Ki; // integral limit

        C610_PID_pos[idx].d = (C610_PID_pos[idx].decurr - C610_PID_pos[idx].deprev) * FREQ_CTRL * C610_PID_pos[idx].Kd; // derivative filtering

        C610[idx].ctrl.spd = C610_PID_pos[idx].p + C610_PID_pos[idx].i + C610_PID_pos[idx].d;
    }
    C610_SetSpd(CAN_handle, C610_ID);
}

void C610_SetTrq(CAN_handle_t *const CAN_handle, const unsigned short C610_ID) {
    for (unsigned char idx = (C610_ID == C610_ID2 ? 4 : 0); idx < (C610_ID == C610_ID1 ? 4 : 8); ++idx)
        C610[idx].ctrl.curr = C610[idx].ctrl.trq / M2006_fTRQ;

    C610_SetCurr(CAN_handle, C610_ID);
}

bool C610_MsgHandler(unsigned CAN_ID, unsigned char RxData[8]) {
    if (CAN_ID >= 0x201 && CAN_ID <= 0x208) {
        static float pos_prev[8], pos_curr[8];
        unsigned char idx = CAN_ID - 0x201;

        pos_curr[idx] = ((RxData[0] << 8) | RxData[1]) * C610_fPOS;
        if (ABS(pos_prev[idx] - pos_curr[idx]) >= 180) // lap
            C610[idx].fdbk.pos += ((pos_prev[idx] > pos_curr[idx] ? 360 : -360) + pos_curr[idx] - pos_prev[idx]) / M2006_GR;
        else
            C610[idx].fdbk.pos += (pos_curr[idx] - pos_prev[idx]) / M2006_GR;
        pos_prev[idx] = pos_curr[idx];

        C610[idx].fdbk.spd = (short)((RxData[2] << 8) | RxData[3]) / M2006_GR;
        C610[idx].fdbk.curr = (short)((RxData[4] << 8) | RxData[5]) * C610_fCURR;
        C610[idx].fdbk.trq = C610[idx].fdbk.curr * M2006_fTRQ;

        C610_PID_spd[idx].deprev = C610_PID_spd[idx].decurr;
        C610_PID_spd[idx].decurr = C610[idx].ctrl.spd - C610[idx].fdbk.spd;

        C610_PID_pos[idx].deprev = C610_PID_pos[idx].decurr;
        C610_PID_pos[idx].decurr = C610[idx].ctrl.pos - C610[idx].fdbk.pos;

        return false;
    }
    return true;
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

void C620_SetCurr(CAN_handle_t *const CAN_handle, const unsigned short C620_ID) {
    unsigned char TxData[C620_ID == (C620_ID1 | C620_ID2) ? 16 : 8];

    for (unsigned char idx = (C620_ID == C620_ID2 ? 4 : 0), offset = idx; idx < (C620_ID == C620_ID1 ? 4 : 8); ++idx) {
        TxData[(idx - offset) * 2] = (short)(LIMIT_ABS(C620[idx].ctrl.curr, C620_CURR_LIMIT) / C620_fCURR) >> 8;
        TxData[(idx - offset) * 2 + 1] = (short)(C620[idx].ctrl.curr / C620_fCURR);
    }

    if (C620_ID == (C620_ID1 | C620_ID2)) {
#ifdef CAN_SUPPORT
        CAN_SendData(CAN_handle, CAN_ID_STD, C620_ID1, TxData, 8);
        CAN_SendData(CAN_handle, CAN_ID_STD, C620_ID2, &TxData[8], 8);
#elif defined FDCAN_SUPPORT
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C620_ID1, TxData, 8);
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C620_ID2, &TxData[8], 8);
#endif
    } else
#ifdef CAN_SUPPORT
        CAN_SendData(CAN_handle, CAN_ID_STD, C620_ID, TxData, 8);
#elif defined FDCAN_SUPPORT
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C620_ID, TxData, 8);
#endif
}

void C620_SetSpd(CAN_handle_t *const CAN_handle, const unsigned short C620_ID) {
    static float iterm[8];

    for (unsigned char idx = (C620_ID == C620_ID2 ? 4 : 0); idx < (C620_ID == C620_ID1 ? 4 : 8); ++idx) {
        C620_PID_spd[idx].p = (C620[idx].ctrl.spd - C620[idx].fdbk.spd) * C620_PID_spd[idx].Kp;

        if (ABS(C620[idx].ctrl.spd - C620[idx].fdbk.spd) >= C620_PID_spd[idx].i_start) // integral separation
            iterm[idx] *= 0.4;
        else
            iterm[idx] += (C620[idx].ctrl.spd - C620[idx].fdbk.spd) / FREQ_CTRL;

        C620_PID_spd[idx].i = LIMIT_ABS(iterm[idx], C620_PID_spd[idx].i_lim) * C620_PID_spd[idx].Ki; // integral limit

        C620_PID_spd[idx].d = (C620_PID_spd[idx].decurr - C620_PID_spd[idx].deprev) * FREQ_CTRL * C620_PID_spd[idx].Kd; // derivative filtering

        C620[idx].ctrl.curr = C620_PID_spd[idx].p + C620_PID_spd[idx].i + C620_PID_spd[idx].d;
    }
    C620_SetCurr(CAN_handle, C620_ID);
}

void C620_SetPos(CAN_handle_t *const CAN_handle, const unsigned short C620_ID) {
    static float iterm[8];

    for (unsigned char idx = (C620_ID == C620_ID2 ? 4 : 0); idx < (C620_ID == C620_ID1 ? 4 : 8); ++idx) {
        C620_PID_pos[idx].p = (C620[idx].ctrl.pos - C620[idx].fdbk.pos) * C620_PID_pos[idx].Kp;

        if (ABS(C620[idx].ctrl.pos - C620[idx].fdbk.pos) >= C620_PID_pos[idx].i_start) // integral separation
            iterm[idx] = 0;
        else
            iterm[idx] += (C620[idx].ctrl.pos - C620[idx].fdbk.pos) / FREQ_CTRL;

        C620_PID_pos[idx].i = LIMIT_ABS(iterm[idx], C620_PID_pos[idx].i_lim) * C620_PID_pos[idx].Ki; // integral limit

        C620_PID_pos[idx].d = (C620_PID_pos[idx].decurr - C620_PID_pos[idx].deprev) * FREQ_CTRL * C620_PID_pos[idx].Kd; // derivative filtering

        C620[idx].ctrl.spd = C620_PID_pos[idx].p + C620_PID_pos[idx].i + C620_PID_pos[idx].d;
    }
    C620_SetSpd(CAN_handle, C620_ID);
}

void C620_SetTrq(CAN_handle_t *const CAN_handle, const unsigned short C620_ID) {
    for (unsigned char idx = (C620_ID == C620_ID2 ? 4 : 0); idx < (C620_ID == C620_ID1 ? 4 : 8); ++idx)
        C620[idx].ctrl.curr = C620[idx].ctrl.trq / M3508_fTRQ;

    C620_SetCurr(CAN_handle, C620_ID);
}

bool C620_MsgHandler(unsigned CAN_ID, unsigned char RxData[8]) {
    if (CAN_ID >= 0x201 && CAN_ID <= 0x208) {
        static float pos_prev[8], pos_curr[8];
        unsigned char idx = CAN_ID - 0x201;

        pos_curr[idx] = ((RxData[0] << 8) | RxData[1]) * C620_fPOS;
        if (ABS(pos_prev[idx] - pos_curr[idx]) >= 180) // lap count
            C620[idx].fdbk.pos += ((pos_prev[idx] > pos_curr[idx] ? 360 : -360) - pos_prev[idx] + pos_curr[idx]) / M3508_GR;
        else
            C620[idx].fdbk.pos += (pos_curr[idx] - pos_prev[idx]) / M3508_GR;
        pos_prev[idx] = pos_curr[idx];

        C620[idx].fdbk.spd = (short)((RxData[2] << 8) | RxData[3]) / M3508_GR;
        C620[idx].fdbk.curr = (short)((RxData[4] << 8) | RxData[5]) * C620_fCURR;
        C620[idx].fdbk.trq = C620[idx].fdbk.curr * M3508_fTRQ;
        C620[idx].fdbk.temp = RxData[6];

        C620_PID_spd[idx].deprev = C620_PID_spd[idx].decurr;
        C620_PID_spd[idx].decurr = C620[idx].ctrl.spd - C620[idx].fdbk.spd;

        C620_PID_pos[idx].deprev = C620_PID_pos[idx].decurr;
        C620_PID_pos[idx].decurr = C620[idx].ctrl.pos - C620[idx].fdbk.pos;

        return false;
    }
    return true;
}

#else
#error FREQ_CTRL undefined
#endif

#endif