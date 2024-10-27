#ifndef __C610_H
#define __C610_H

#define C610_MODE_ANGLE

#define C610_fCURRENT (10 / 10000.F)
#define C610_fANGLE (360 / 8192.F)
#define C610_fTORQUE 0.18
#define C610_GR 36.F
#define C610_CURRENT_MAX 10
#define C610_DPS_MAX 3.6

#define C610_ID1 0x200
#define C610_ID2 0x1FF

#define C610_ANGLE_Kp 16
#define C610_ANGLE_Ki 10
#define C610_ANGLE_Kd 1
#define C610_ANGLE_iSTART 60
#define C610_ANGLE_iLIMIT 0.2

#define C610_RPM_Kp 0.8
#define C610_RPM_Ki 0.25
#define C610_RPM_Kd 2.7e-3
#define C610_RPM_iSTART 100
#define C610_RPM_iLIMIT 5

struct C610
{
	float Angle[8];
	float RPM[8];
	float Current[8];
	float Torque[8];
};

#ifdef C610_MODE_ANGLE
void C610_PID_Angle(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);
#endif
#ifdef C610_MODE_RPM
void C610_PID_RPM(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);
#endif
void C610_SetTorque(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);

#endif