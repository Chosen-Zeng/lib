#ifndef __C620_H
#define __C620_H

#define MODE	//mode selection: RPM_MODE/ANGLE_MODE

#define fANGLE_C620 (360 / 8192.F)
#define fI_C620 (20 / 16384.F)
#define GR_C620 (3591.f / 187)

#define C620_ID1 0x200
#define C620_ID2 0x1FF

//PID param for STM32G474VET6 on FreeRTOS
#define ANGLE_Kp 24
#define ANGLE_Ki 12.5
#define ANGLE_Kd 2.4e-2
#define ANGLE_pLIMIT 100
#define ANGLE_iSTART 35
#define ANGLE_iLIMIT 80
#define ANGLE_dLIMIT 120

#define RPM_Kp 2.4e-2
#define RPM_Ki 1e-2
#define RPM_Kd 3.8e-5
#define RPM_pDEADBAND 26
#define RPM_iSTART 275
#define RPM_iLIMIT 25


struct C620
{
	float Angle[8];
	float RPM[8];
	float I[8];
	float Temp[8];
};


#ifdef ANGLE_MODE
void C620_PID_Angle(FDCAN_HandleTypeDef* hfdcan, uint32_t ID);
#elif defined RPM_MODE
void C620_PID_RPM(FDCAN_HandleTypeDef* hfdcan, uint32_t ID);
#else
#warning No mode selected.
#endif

#endif