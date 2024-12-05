#ifndef __RM2006_H
#define __RM2006_H

#include "main.h"

#define C610_MODE_ANGLE

#define M2006_fTORQUE 0.3
#define M2006_GR 36.F

#define C610_fCURRENT (10.F / 10000)
#define C610_fANGLE (360.F / 8192)
#define C610_CURRENT_LIMIT 10

#define C610_ID1 0x200
#define C610_ID2 0x1FF

#define C610_ANGLE_Kp 10 //16
#define C610_ANGLE_Ki 12
#define C610_ANGLE_Kd 0.1 // 1
#define C610_ANGLE_iSTART 60
#define C610_ANGLE_iLIMIT 0.8

#define C610_RPM_Kp 0.4
#define C610_RPM_Ki 0.25
#define C610_RPM_Kd 1e-4 //2.7e-3
#define C610_RPM_iSTART 100
#define C610_RPM_iLIMIT 10//5

typedef enum
{
	C610_TIME_SRC_RPM,
	C610_TIME_SRC_ANGLE
} C610_time_src_t;

typedef struct
{
	float angle[8];
	float RPM[8];
	float current[8];
	float torque[8];
} C610_ctrl_t, C610_fdbk_t;

typedef struct
{
	C610_ctrl_t ctrl;
	C610_fdbk_t fdbk;
	C610_time_src_t time_src;
} C610_t;

typedef struct
{
	float p[8];
	float i[8];
	float d[8];
	float pterm[8];
	float iterm[8];
	float dterm[8];
	float deprev[8];
	float decurr[8];
} C610_PID_t;

extern C610_t C610;
extern C610_PID_t C610_PID_RPM, C610_PID_angle;

void C610_SetCurrent(void *CAN_handle, uint32_t C610_ID);
void C610_SetAngle(void *CAN_handle, uint32_t C610_ID);
void C610_SetRPM(void *CAN_handle, uint32_t C610_ID);
void C610_SetTorque(void *CAN_handle, uint32_t C610_ID);

#endif