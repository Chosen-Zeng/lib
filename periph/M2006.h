#ifndef __M2006_H
#define __M2006_H

#include "user.h"

#ifdef FREQ

#define M2006_fTORQUE 0.3
#define M2006_GR 36.F

#define C610_fCURRENT (10.F / 10000)
#define C610_fPOS (360.F / 8192)
#define C610_CURRENT_LIMIT 10

#define C610_ID1 0x200
#define C610_ID2 0x1FF

#define C610_POS_Kp 10	// 16
#define C610_POS_Ki 10	// 12
#define C610_POS_Kd 0.1 // 1
#define C610_POS_iSTART 60
#define C610_POS_iLIMIT 0.8

#define C610_SPD_Kp 0.4
#define C610_SPD_Ki 0.25
#define C610_SPD_Kd 1e-4 // 2.7e-3
#define C610_SPD_iSTART 100
#define C610_SPD_iLIMIT 10 // 5

typedef struct
{
	float pos;
	float spd;
	float current;
	float torque;
} C610_ctrl_t, C610_fdbk_t;

typedef struct
{
	C610_ctrl_t ctrl;
	C610_fdbk_t fdbk;
} C610_t;

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
} C610_PID_t;

extern C610_t C610[8];
extern C610_PID_t C610_PID_spd[8], C610_PID_pos[8];

void C610_SetCurrent(void *CAN_handle, uint32_t C610_ID);
void C610_SetPos(void *CAN_handle, uint32_t C610_ID);
void C610_SetSpd(void *CAN_handle, uint32_t C610_ID);
void C610_SetTorque(void *CAN_handle, uint32_t C610_ID);

#endif
#endif