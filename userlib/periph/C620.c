#include "FDCAN.h"
#include "stm32g4xx_hal.h"
#include "C620.h"

#define ABS(X) ((X) >= 0 ? (X) : -(X))				  // 输出X绝对值
#define ABS_LIMIT(X, Y) \
    if (ABS(X) > Y)     \
    X >= 0 ? (X = Y) : (X = -Y)

void C620_PID_Angle(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);
void C620_SetCurrent(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);
void C620_PID_RPM(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);

static struct PID
{
	float p[8];
	float i[8];
	float d[8];
	float pterm[8];
	float iterm[8];
	float dterm[8];
	int16_t deprev[8];
	int16_t decurr[8];
} PID_RPM, PID_Angle;
struct C620 C620_CTRL, C620_FDBK;
int16_t C620_lap[8];
static float time;

void C620_PID_Angle(FDCAN_HandleTypeDef *hfdcan, uint32_t ID)
{
#ifdef C620_MODE_ANGLE
	static float time_prev, time_curr;
	time_curr = (float)(TIM6->CNT + 1) / (TIM6->ARR + 1);
	if (time_prev > time_curr) // 时间间隔
		time = 1 + time_curr - time_prev;
	else
		time = time_curr - time_prev;

	time_prev = time_curr;
#endif

	for (int count = 0; count < 8; count++)
	{
		PID_Angle.pterm[count] = C620_CTRL.Angle[count] - (C620_FDBK.Angle[count] + C620_lap[count] * 360); // 对pterm限幅 实现类似步进效果
			ABS_LIMIT(PID_Angle.pterm[count], C620_ANGLE_pLIMIT);
		PID_Angle.p[count] = PID_Angle.pterm[count] * C620_ANGLE_Kp;

		if (ABS(PID_Angle.pterm[count]) >= C620_ANGLE_iSTART) // 积分分离
		{
			PID_Angle.iterm[count] = 0;
		}
		else if (ABS(PID_Angle.iterm[count]) <= C620_ANGLE_iLIMIT) // 积分限幅
		{
			PID_Angle.iterm[count] += PID_Angle.pterm[count] * time;
				ABS_LIMIT(PID_Angle.iterm[count], C620_ANGLE_iLIMIT);
		}
		PID_Angle.i[count] = PID_Angle.iterm[count] * C620_ANGLE_Ki;

		PID_Angle.dterm[count] = (PID_Angle.decurr[count] - PID_Angle.deprev[count]) / time; // 微分先行
		PID_Angle.d[count] = PID_Angle.dterm[count] * C620_ANGLE_Kd;

		C620_CTRL.RPM[count] = PID_Angle.p[count] + PID_Angle.i[count] + PID_Angle.d[count];
	}
	C620_PID_RPM(hfdcan, ID);
}

void C620_PID_RPM(FDCAN_HandleTypeDef *hfdcan, uint32_t ID)
{
#ifdef C620_MODE_RPM
	static float time_prev, time_curr;
	time_curr = (float)(TIM6->CNT + 1) / (TIM6->ARR + 1);
	if (time_prev > time_curr) // 时间间隔
		time = 1 + time_curr - time_prev;
	else
		time = time_curr - time_prev;

	time_prev = time_curr;
#endif

	for (int count = 0; count < 8; count++)
	{
		if (ABS(PID_RPM.pterm[count] = C620_CTRL.RPM[count] - C620_FDBK.RPM[count]) < C620_RPM_pDEADBAND) // 死区
			PID_RPM.pterm[count] = 0;
		PID_RPM.p[count] = PID_RPM.pterm[count] * C620_RPM_Kp;

		if (ABS(PID_RPM.pterm[count]) >= C620_RPM_iSTART) // 积分分离
		{
			PID_RPM.iterm[count] *= 0.4;
		}
		else if (ABS(PID_RPM.iterm[count]) <= C620_RPM_iLIMIT) // 积分限幅
		{
			PID_RPM.iterm[count] += PID_RPM.pterm[count] * time;
				ABS_LIMIT(PID_RPM.iterm[count], C620_RPM_iLIMIT);
		}
		PID_RPM.i[count] = PID_RPM.iterm[count] * C620_RPM_Ki;

		PID_RPM.dterm[count] = (PID_RPM.decurr[count] - PID_RPM.deprev[count]) / time; // 微分先行
		PID_RPM.d[count] = PID_RPM.dterm[count] * C620_RPM_Kd;

		C620_CTRL.Current[count] = PID_RPM.p[count] + PID_RPM.i[count] + PID_RPM.d[count];
	}
	C620_SetI(hfdcan, ID);
}

void C620_SetI(FDCAN_HandleTypeDef *hfdcan, uint32_t ID)
{
	uint8_t C620[16];

	for (int count = 0; count < 8; count++)
	{
			ABS_LIMIT(C620_CTRL.Current[count], C620_CURRENT_MAX);
		C620[count * 2] = (int16_t)(C620_CTRL.Current[count] / C620_fCURRENT) >> 8;
		C620[count * 2 + 1] = (int16_t)(C620_CTRL.Current[count] / C620_fCURRENT);
	}

	FDCAN_SendData(hfdcan, FDCAN_STANDARD_ID, ID, ID == C620_ID1 ? C620 : &C620[8], 8);
}

__weak void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
	uint8_t RxFifo0[8];
	if (hfdcan->Instance == FDCAN1)
	{
		HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxFifo0);
		uint8_t count = FDCAN_RxHeader.Identifier - 0x200 - 1;

		C620_FDBK.Angle[count] = (int16_t)((RxFifo0[0] << 8) | RxFifo0[1]) * C620_fANGLE;
		C620_FDBK.RPM[count] = (int16_t)((RxFifo0[2] << 8) | RxFifo0[3]);
		C620_FDBK.Current[count] = (int16_t)((RxFifo0[4] << 8) | RxFifo0[5]) * C620_fCURRENT;
		C620_FDBK.Temp[count] = RxFifo0[6];

		static int16_t angle_prev[8];

		if (ABS(angle_prev[count] - C620_FDBK.Angle[count]) >= C620_DPS_MAX) // 计圈
			angle_prev[count] > C620_FDBK.Angle[count] ? C620_lap[count]++ : C620_lap[count]--;
		angle_prev[count] = C620_FDBK.Angle[count];

		PID_RPM.deprev[count] = PID_RPM.decurr[count];
		PID_RPM.decurr[count] = C620_CTRL.RPM[count] - C620_FDBK.RPM[count];

#ifdef C620_MODE_ANGLE
		PID_Angle.deprev[count] = PID_Angle.decurr[count];
		if (ABS(PID_Angle.decurr[count] = C620_CTRL.Angle[count] - (C620_FDBK.Angle[count] + C620_lap[count] * 360)) > C620_ANGLE_dLIMIT) // 微分限幅
			ABS_LIMIT(PID_Angle.decurr[count], C620_ANGLE_dLIMIT);
#endif
	}
}