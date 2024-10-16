#ifndef __C610_H
#define __C610_H

#define C610_MODE // mode selection: C610_MODE_ANGLE/C610_MODE_RPM/C610_MODE_T?

#define C610_fI (10 / 10000.F)
#define C610_fANGLE (360 / 8192.F)
#define C610_GR 36
#define C610_I_MAX 10
#define C610_DPS_MAX 120

#define C610_ID1 0x200
#define C610_ID2 0x1FF

// PID param for STM32G474VET6 on FreeRTOS
#define C610_ANGLE_Kp 12
#define C610_ANGLE_Ki 15
#define C610_ANGLE_Kd 8e-3
#define C610_ANGLE_pLIMIT 1500
#define C610_ANGLE_iSTART 75
#define C610_ANGLE_iLIMIT 50
#define C610_ANGLE_dLIMIT 500

#define C610_RPM_Kp 1.7e-2
#define C610_RPM_Ki 1e-2
#define C610_RPM_Kd 1.9e-4
#define C610_RPM_iSTART 3000
#define C610_RPM_iLIMIT 75

struct C610
{
	float Angle[8];
	float RPM[8];
	float I[8];
	float T[8];
};

#ifdef C610_MODE_ANGLE
void C610_PID_Angle(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);
#elif defined C610_MODE_RPM
void C610_PID_RPM(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);
#endif

#endif