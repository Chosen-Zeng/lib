#ifndef __ROBOMASTER_H
#define __ROBOMASTER_H

#include "usr.h"

#ifdef FREQ_CTRL
typedef struct {
    float Kp,
        p,
        Ki,
        i,
        i_start,
        i_lim,
        Kd,
        d,
        deprev,
        decurr;
} PID_t;

#define M2006_fTRQ 0.18f
#define M2006_GR   36.f

#define C610_fCURR (10.f / 10000)
#define C610_fPOS  (360.f / 8192)

#define C610_CURR_LIMIT 10

#define C610_ID1 0x200
#define C610_ID2 0x1FF

extern PID_t C610_PID_spd[C610_NUM], C610_PID_pos[C610_NUM];

typedef struct {
    struct {
        float pos,
            spd,
            curr,
            trq;
    } ctrl,
        fdbk;
} C610_t;
extern C610_t C610[C610_NUM];

void C610_SetPos(CAN_handle_t *const CAN_handle, const unsigned short C610_ID);
void C610_SetSpd(CAN_handle_t *const CAN_handle, const unsigned short C610_ID);
void C610_SetTrq(CAN_handle_t *const CAN_handle, const unsigned short C610_ID);

bool C610_MsgHandler(unsigned CAN_ID, unsigned char RxData[8]);

#define M3508_fTRQ 0.3f
#define M3508_GR   (3591.f / 187)

#define C620_fPOS  (360.f / 8192)
#define C620_fCURR (20.f / 16384)

#define C620_CURR_LIMIT 20

#define C620_ID1 0x200
#define C620_ID2 0x1FF

extern PID_t C620_PID_spd[C620_NUM], C620_PID_pos[C620_NUM];

typedef struct {
    struct {
        float pos,
            spd,
            trq,
            curr;
    } ctrl;
    struct {
        float pos,
            spd, trq,
            curr,
            temp;
    } fdbk;
} C620_t;
extern C620_t C620[C620_NUM];

void C620_SetPos(CAN_handle_t *const CAN_handle, const unsigned short C620_ID);
void C620_SetSpd(CAN_handle_t *const CAN_handle, const unsigned short C620_ID);
void C620_SetTrq(CAN_handle_t *const CAN_handle, const unsigned short C620_ID);

bool C620_MsgHandler(unsigned CAN_ID, unsigned char RxData[8]);

#endif
#endif