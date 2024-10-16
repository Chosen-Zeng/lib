#ifndef __C620_H
#define __C620_H

#define C620_MODE	//mode selection: C620_MODE_ANGLE/C620_MODE_RPM


#define C620_fANGLE (360 / 8192.F)
#define C620_fI (20 / 16384.F)
#define C620_GR (3591 / 187.F)
#define C620_I_MAX 20
#define C620_DPS_MAX 60

#define C620_ID1 0x200
#define C620_ID2 0x1FF

//PID param for STM32G474VET6 on FreeRTOS
#define C620_ANGLE_Kp 24
#define C620_ANGLE_Ki 12.5
#define C620_ANGLE_Kd 2.4e-2
#define C620_ANGLE_pLIMIT 100
#define C620_ANGLE_iSTART 50
#define C620_ANGLE_iLIMIT 80
#define C620_ANGLE_dLIMIT 120

#define C620_RPM_Kp 2.4e-2
#define C620_RPM_Ki 1e-2
#define C620_RPM_Kd 3.8e-5
#define C620_RPM_pDEADBAND 26
#define C620_RPM_iSTART 275
#define C620_RPM_iLIMIT 25


struct C620
{
	float Angle[8];
	float RPM[8];
	float I[8];
	float Temp[8];
};


#ifdef C620_MODE_ANGLE
void C620_PID_Angle(FDCAN_HandleTypeDef* hfdcan, uint32_t ID);
#elif defined C620_MODE_RPM
void C620_PID_RPM(FDCAN_HandleTypeDef* hfdcan, uint32_t ID);
#else
#warning No mode selected.
#endif

#endif