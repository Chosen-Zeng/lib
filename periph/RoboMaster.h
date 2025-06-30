#ifndef __ROBOMASTER_H
#define __ROBOMASTER_H

#include "user.h"

#ifdef FREQ_CTRL

#define M2006_fTRQ 0.18f
#define M2006_GR 36.f

#define C610_fCURR (10.f / 10000)
#define C610_fPOS (360.f / 8192)

#define C610_POS_Kp 10 // 16
#define C610_POS_Ki 12
#define C610_POS_Kd 0.01f // 1
#define C610_POS_iSTART 60
#define C610_POS_iLIMIT 0.8f

#define C610_SPD_Kp 0.4f
#define C610_SPD_Ki 0.25f
#define C610_SPD_Kd 1e-4f // 2.7e-3f
#define C610_SPD_iSTART 100
#define C610_SPD_iLIMIT 10 // 5

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

#define C620_POS_Kp 19
#define C620_POS_Ki 3
#define C620_POS_Kd 0.05f // 0.35f
#define C620_POS_iSTART 2
#define C620_POS_iLIMIT 2

#define C620_SPD_Kp 0.4f
#define C620_SPD_Ki 0.6f
#define C620_SPD_Kd 0.0015f
#define C620_SPD_iSTART 50
#define C620_SPD_iLIMIT 10

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
	float p, i, d, deprev, decurr;
} PID_t;
extern PID_t C610_PID_spd[8], C610_PID_pos[8], C620_PID_spd[8], C620_PID_pos[8];

void C620_SetPos(void *CAN_handle, unsigned short C620_ID);
void C620_SetSpd(void *CAN_handle, unsigned short C620_ID);
void C620_SetTrq(void *CAN_handle, unsigned short C620_ID);

#endif
#endif