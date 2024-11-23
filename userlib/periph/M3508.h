#ifndef __RM3508_H
#define __RM3508_H

#include "main.h"

#define C620_MODE_ANGLE // mode selection: C620_MODE_ANGLE/C620_MODE_RPM


#define M3508_fTORQUE 0.3
#define M3508_GR (3591.F / 187)

#define C620_fANGLE (360.F / 8192)
#define C620_fCURRENT (20.F / 16384)
#define C620_CURRENT_LIMIT 20

#define C620_ID1 0x200
#define C620_ID2 0x1FF

// PID param for STM32G474VET6 on FreeRTOS
#define C620_ANGLE_Kp 19
#define C620_ANGLE_Ki 3
#define C620_ANGLE_Kd 0.35
#define C620_ANGLE_iSTART 2
#define C620_ANGLE_iLIMIT 2

#define C620_RPM_Kp 0.4
#define C620_RPM_Ki 0.6
#define C620_RPM_Kd 0.0015
#define C620_RPM_iSTART 50
#define C620_RPM_iLIMIT 10

typedef enum
{
	C620_TIME_SRC_RPM,
	C620_TIME_SRC_ANGLE
} C620_time_src_t;

typedef struct
{
	float angle[8];
	float RPM[8];
	float torque[8];
	float current[8];
} C620_ctrl_t;

typedef struct
{
	float angle[8];
	float RPM[8];
	float torque[8];
	float current[8];
	float temp[8];
} C620_fdbk_t;

typedef struct
{
	C620_ctrl_t ctrl;
	C620_fdbk_t fdbk;
	C620_time_src_t time_src;
} C620_t;


extern C620_t C620;

void C620_SetAngle(void *CAN_handle, uint32_t C620_ID);
void C620_SetRPM(void *CAN_handle, uint32_t C620_ID);
void C620_SetTorque(void *CAN_handle, uint32_t C620_ID);

#endif