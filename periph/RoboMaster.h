#ifndef __ROBOMASTER_H
#define __ROBOMASTER_H

#include "usr.h"

#ifdef FREQ_CTRL

#define M2006_fTRQ 0.18f
#define M2006_GR 36.f

#define C610_fCURR (10.f / 10000)
#define C610_fPOS (360.f / 8192)

#define C610_CURR_LIMIT 10

#define C610_ID1 0x200
#define C610_ID2 0x1FF

typedef struct
{
	struct
	{
		float pos, spd, curr, trq;
	} ctrl, fdbk;
} C610_t;
extern C610_t C610[8];

void C610_SetPos(void *CAN_handle, unsigned short C610_ID);
void C610_SetSpd(void *CAN_handle, unsigned short C610_ID);
void C610_SetTrq(void *CAN_handle, unsigned short C610_ID);

#define M3508_fTRQ 0.3f
#define M3508_GR (3591.f / 187)

#define C620_fPOS (360.f / 8192)
#define C620_fCURR (20.f / 16384)

#define C620_CURR_LIMIT 20

#define C620_ID1 0x200
#define C620_ID2 0x1FF

typedef struct
{
	struct
	{
		float pos, spd, trq, curr;
	} ctrl;
	struct
	{
		float pos, spd, trq, curr, temp;
	} fdbk;
} C620_t;
extern C620_t C620[8];

typedef struct
{
	float Kp, p, Ki, i, i_start, i_lim, Kd, d, deprev, decurr;
} PID_t;
extern PID_t C610_PID_spd[8], C610_PID_pos[8], C620_PID_spd[8], C620_PID_pos[8];

void C620_SetPos(void *CAN_handle, unsigned short C620_ID);
void C620_SetSpd(void *CAN_handle, unsigned short C620_ID);
void C620_SetTrq(void *CAN_handle, unsigned short C620_ID);

#endif
#endif