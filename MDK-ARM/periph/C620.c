#include "FDCAN.h"
#include "stm32g4xx_hal.h"
#include "C620.h"

#include <string.h>
#include <stdio.h>

#define ABS(X) ((X) >= 0 ? (X) : -(X))				  // 输出X绝对值
#define ABS_LIMIT(X, Y) (X >= 0 ? (X = Y) : (X = -Y)) // 将X限制为±Y

extern FDCAN_HandleTypeDef hfdcan1;

void C620_PID_Angle(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);
void C620_SetI(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);
void C620_PID_RPM(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);

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

struct PID RPM, Angle;
struct C620 C620_CTRL, C620_FDBK;
int16_t lap[8];
static uint8_t C620[8];
static float time;
static uint8_t RxFifo0[8];

void C620_PID_Angle(FDCAN_HandleTypeDef *hfdcan, uint32_t ID)
{
	time = TIM6->CNT / 4000.f; // 时间间隔
	TIM6->CNT = 0;

	for (int count = 0; count < 8; count++)
	{

		if (ABS(Angle.pterm[count] = C620_CTRL.Angle[count] - (C620_FDBK.Angle[count] + lap[count] * 360)) > ANGLE_pLIMIT) // 对pterm限幅 实现类似步进效果
			ABS_LIMIT(Angle.pterm[count], ANGLE_pLIMIT);
		Angle.p[count] = Angle.pterm[count] * ANGLE_Kp;

		if (ABS(Angle.pterm[count]) >= ANGLE_iSTART) // 积分分离
		{
			Angle.iterm[count] = 0;
		}
		else if (ABS(Angle.iterm[count]) <= ANGLE_iLIMIT) // 积分限幅
		{
			Angle.iterm[count] += Angle.pterm[count] * time;
			if (ABS(Angle.iterm[count]) > ANGLE_iLIMIT)
				ABS_LIMIT(Angle.iterm[count], ANGLE_iLIMIT);
		}
		Angle.i[count] = Angle.iterm[count] * ANGLE_Ki;

		Angle.dterm[count] = (Angle.decurr[count] - Angle.deprev[count]) / time; // 微分先行
		Angle.d[count] = Angle.dterm[count] * ANGLE_Kd;

		C620_CTRL.RPM[count] = Angle.p[count] + Angle.i[count] + Angle.d[count];
	}
	C620_PID_RPM(hfdcan, ID);
}

void C620_PID_RPM(FDCAN_HandleTypeDef *hfdcan, uint32_t ID)
{
#ifdef RPM_MODE
	time = TIM6->CNT / 4000.f;
	TIM6->CNT = 0;
#endif

	for (int count = 0; count < 8; count++)
	{
		if (ABS(RPM.pterm[count] = C620_CTRL.RPM[count] - C620_FDBK.RPM[count]) < RPM_pDEADBAND) // 死区
			RPM.pterm[count] = 0;
		RPM.p[count] = RPM.pterm[count] * RPM_Kp;

		if (ABS(RPM.pterm[count]) >= RPM_iSTART) // 积分分离
		{
			RPM.iterm[count] *= 0.4;
		}
		else if (ABS(RPM.iterm[count]) <= RPM_iLIMIT) // 积分限幅
		{
			RPM.iterm[count] += RPM.pterm[count] * time;
			if (ABS(RPM.iterm[count]) > RPM_iLIMIT)
				ABS_LIMIT(RPM.iterm[count], RPM_iLIMIT);
		}
		RPM.i[count] = RPM.iterm[count] * RPM_Ki;

		RPM.dterm[count] = (RPM.decurr[count] - RPM.deprev[count]) / time; // 微分先行
		RPM.d[count] = RPM.dterm[count] * RPM_Kd;

		C620_CTRL.I[count] = RPM.p[count] + RPM.i[count] + RPM.d[count];
	}
	C620_SetI(hfdcan, ID);
}

void C620_SetI(FDCAN_HandleTypeDef *hfdcan, uint32_t ID)
{
	for (int count = 0; count <= 3; count++)
	{
		if (ABS(C620_CTRL.I[count]) > 20)
			ABS_LIMIT(C620_CTRL.I[count], 20);
		C620[count * 2] = (int16_t)(C620_CTRL.I[count] / 20 * 16384) >> 8;
		C620[count * 2 + 1] = (int16_t)(C620_CTRL.I[count] / 20 * 16384);
	}

	FDCAN_SendData(hfdcan, ID, C620);
}
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
	if (hfdcan->Instance == FDCAN1)
	{
		HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxFifo0);
		int8_t count = FDCAN_RxHeader.Identifier - 0x200 - 1;

		C620_FDBK.Angle[count] = (int16_t)((RxFifo0[0] << 8) | RxFifo0[1]) * fAngle;
		C620_FDBK.RPM[count] = (int16_t)((RxFifo0[2] << 8) | RxFifo0[3]);
		C620_FDBK.I[count] = (int16_t)((RxFifo0[4] << 8) | RxFifo0[5]) * fI;
		C620_FDBK.Temp[count] = RxFifo0[6];

		static int16_t angle[8];

		if (ABS(angle[count] - C620_FDBK.Angle[count]) >= 60) // 计圈
			angle[count] > C620_FDBK.Angle[count] ? lap[count]++ : lap[count]--;
		angle[count] = C620_FDBK.Angle[count];

		RPM.deprev[count] = RPM.decurr[count];
		RPM.decurr[count] = C620_CTRL.RPM[count] - C620_FDBK.RPM[count];

#ifdef ANGLE_MODE
		Angle.deprev[count] = Angle.decurr[count];
		if (ABS(Angle.decurr[count] = C620_CTRL.Angle[count] - (C620_FDBK.Angle[count] + lap[count] * 360)) > 120) // 微分限幅
			ABS_LIMIT(Angle.decurr[count], 120);
#endif
	}
}