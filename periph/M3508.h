#ifndef __M3508_H
#define __M3508_H

#include "user.h"

#ifdef FREQ_CTRL

#define M3508_fTRQ 0.3
#define M3508_GR (3591.F / 187)

#define C620_fPOS (360.F / 8192)
#define C620_fCURR (20.F / 16384)

#define C620_POS_Kp 19
#define C620_POS_Ki 3
#define C620_POS_Kd 0.05 // 0.35
#define C620_POS_iSTART 2
#define C620_POS_iLIMIT 2

#define C620_SPD_Kp 0.4
#define C620_SPD_Ki 0.6
#define C620_SPD_Kd 0.0015
#define C620_SPD_iSTART 50
#define C620_SPD_iLIMIT 10

#define C620_CURR_LIMIT 20

#define C620_ID1 0x200
#define C620_ID2 0x1FF

typedef struct
{
	float pos;
	float spd;
	float trq;
	float curr;
} C620_ctrl_t;

typedef struct
{
	float pos;
	float spd;
	float trq;
	float curr;
	float temp;
} C620_fdbk_t;

typedef struct
{
	C620_ctrl_t ctrl;
	C620_fdbk_t fdbk;
} C620_t;
extern C620_t C620[8];

typedef struct
{
	float p;
	float i;
	float d;
	float pterm;
	float iterm;
	float dterm;
	float deprev;
	float decurr;
} C620_PID_t;
extern C620_PID_t C620_PID_spd[8], C620_PID_pos[8];

void C620_SetPos(void *CAN_handle, unsigned short C620_ID);
void C620_SetSpd(void *CAN_handle, unsigned short C620_ID);
void C620_SetTrq(void *CAN_handle, unsigned short C620_ID);

#endif
#endif