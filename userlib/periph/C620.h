#ifndef __C620_H
#define __C620_H

#define ANGLE_MODE	//mode selection: RPM_MODE/ANGLE_MODE

#define C620_ID1 0x200
#define C620_ID2 0x1FF


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
#error No mode selected.
#endif

#endif