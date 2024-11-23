#include "M3508.h"
#include "TIM.h"
#include "algorithm.h"

#ifdef CAN_SUPPORT
#include "CAN.h"
#elif defined FDCAN_SUPPORT
#include "FDCAN.h"
#endif

static struct PID
{
	float p[8];
	float i[8];
	float d[8];
	float pterm[8];
	float iterm[8];
	float dterm[8];
	float deprev[8];
	float decurr[8];
} C620_PID_RPM, C620_PID_angle;

C620_t C620 = {
	.time_src =
#ifdef C620_MODE_RPM
		C620_TIME_SRC_RPM
#elif defined C620_MODE_ANGLE
		C620_TIME_SRC_ANGLE
#endif
};

static time_t C620_time;

void C620_SetCurrent(void *CAN_handle, uint32_t C620_ID)
{
	uint8_t TxData[16];

	for (int count = (C620_ID == C620_ID2 ? 4 : 0); count < (C620_ID == C620_ID1 ? 4 : 8); count++)
	{
		ABS_LIMIT(C620.ctrl.current[count], C620_CURRENT_LIMIT)
		TxData[count * 2] = (int16_t)(C620.ctrl.current[count] / C620_fCURRENT) >> 8;
		TxData[count * 2 + 1] = (int16_t)(C620.ctrl.current[count] / C620_fCURRENT);
	}

	if (C620_ID == (C620_ID1 | C620_ID2))
	{
#ifdef CAN_SUPPORT
		CAN_SendData(CAN_handle, CAN_ID_STD, C620_ID1, TxData, 8);
		CAN_SendData(CAN_handle, CAN_ID_STD, C620_ID2, &TxData[8], 8);
#elif defined FDCAN_SUPPORT
		FDCAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C620_ID1, TxData, 8);
		FDCAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C620_ID2, &TxData[8], 8);
#endif
	}
	else
#ifdef CAN_SUPPORT
		CAN_SendData(CAN_handle, CAN_ID_STD, C620_ID, C620_ID == C620_ID1 ? TxData : &TxData[8], 8);
#elif defined FDCAN_SUPPORT
		FDCAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C620_ID, C620_ID == C620_ID1 ? TxData : &TxData[8], 8);
#endif
}

void C620_SetRPM(void *CAN_handle, uint32_t C620_ID)
{
	if (C620.time_src == C620_TIME_SRC_RPM) // judge the time source(from lower or upper level)
		TIMSW_UpdateInterval(&C620_time);

	for (int count = (C620_ID == C620_ID2 ? 4 : 0); count < (C620_ID == C620_ID1 ? 4 : 8); count++)
	{
		C620_PID_RPM.pterm[count] = C620.ctrl.RPM[count] - C620.fdbk.RPM[count];
		C620_PID_RPM.p[count] = C620_PID_RPM.pterm[count] * C620_RPM_Kp;

		if (ABS(C620_PID_RPM.pterm[count]) >= C620_RPM_iSTART) // 积分分离
		{
			C620_PID_RPM.iterm[count] *= 0.4;
		}
		else if (ABS(C620_PID_RPM.iterm[count]) <= C620_RPM_iLIMIT) // 积分限幅
		{
			C620_PID_RPM.iterm[count] += C620_PID_RPM.pterm[count] * C620_time.interval;
			ABS_LIMIT(C620_PID_RPM.iterm[count], C620_RPM_iLIMIT);
		}
		C620_PID_RPM.i[count] = C620_PID_RPM.iterm[count] * C620_RPM_Ki;

		if (C620_time.interval)
		{
			C620_PID_RPM.dterm[count] = (C620_PID_RPM.decurr[count] - C620_PID_RPM.deprev[count]) / C620_time.interval; // 微分先行
			C620_PID_RPM.d[count] = C620_PID_RPM.dterm[count] * C620_RPM_Kd;
		}

		C620.ctrl.current[count] = C620_PID_RPM.p[count] + C620_PID_RPM.i[count] + C620_PID_RPM.d[count];
	}
	C620_SetCurrent(CAN_handle, C620_ID);
}

void C620_SetAngle(void *CAN_handle, uint32_t C620_ID)
{
	if (C620.time_src == C620_TIME_SRC_ANGLE) // judge the time source(from lower or upper level)
		TIMSW_UpdateInterval(&C620_time);

	for (int count = (C620_ID == C620_ID2 ? 4 : 0); count < (C620_ID == C620_ID1 ? 4 : 8); count++)
	{
		C620_PID_angle.pterm[count] = C620.ctrl.angle[count] - C620.fdbk.angle[count];
		C620_PID_angle.p[count] = C620_PID_angle.pterm[count] * C620_ANGLE_Kp;

		if (ABS(C620_PID_angle.pterm[count]) >= C620_ANGLE_iSTART) // 积分分离
		{
			C620_PID_angle.iterm[count] = 0;
		}
		else if (ABS(C620_PID_angle.iterm[count]) <= C620_ANGLE_iLIMIT) // 积分限幅
		{
			C620_PID_angle.iterm[count] += C620_PID_angle.pterm[count] * C620_time.interval;
			ABS_LIMIT(C620_PID_angle.iterm[count], C620_ANGLE_iLIMIT);
		}
		C620_PID_angle.i[count] = C620_PID_angle.iterm[count] * C620_ANGLE_Ki;

		if (C620_time.interval)
		{
			C620_PID_angle.dterm[count] = (C620_PID_angle.decurr[count] - C620_PID_angle.deprev[count]) / C620_time.interval; // 微分先行
			C620_PID_angle.d[count] = C620_PID_angle.dterm[count] * C620_ANGLE_Kd;
		}
		C620.ctrl.RPM[count] = C620_PID_angle.p[count] + C620_PID_angle.i[count] + C620_PID_angle.d[count];
	}
	C620_SetRPM(CAN_handle, C620_ID);
}

void C620_SetTorque(void *CAN_handle, uint32_t C620_ID)
{
	for (int count = (C620_ID == C620_ID2 ? 4 : 0); count < (C620_ID == C620_ID1 ? 4 : 8); count++)
	{
		C620.ctrl.current[count] = C620.ctrl.torque[count] / M3508_fTORQUE;
	}
	C620_SetCurrent(CAN_handle, C620_ID);
}

#ifdef CAN_SUPPORT
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef CAN_RxHeader;
	uint8_t RxFifo0[8];

	if (hcan->Instance == CAN1)
	{
		HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_RxHeader, RxFifo0);
		uint8_t count = CAN_RxHeader.StdId - 0x200 - 1;
		static float angle_prev[8], angle_curr[8];

		angle_curr[count] = ((RxFifo0[0] << 8) | RxFifo0[1]) * C620_fANGLE;
		if (ABS(angle_prev[count] - angle_curr[count]) >= 180) // 计圈
			C620.fdbk.angle[count] += ((angle_prev[count] > angle_curr[count] ? 360 : -360) - angle_prev[count] + angle_curr[count]) / M3508_GR;
		else
			C620.fdbk.angle[count] += (angle_curr[count] - angle_prev[count]) / M3508_GR;
		angle_prev[count] = angle_curr[count];

		C620.fdbk.RPM[count] = (int16_t)((RxFifo0[2] << 8) | RxFifo0[3]) / M3508_GR;
		C620.fdbk.current[count] = (int16_t)((RxFifo0[4] << 8) | RxFifo0[5]) * C620_fCURRENT;
		C620.fdbk.torque[count] = C620.fdbk.current[count] * M3508_fTORQUE;
		C620.fdbk.temp[count] = RxFifo0[6];

		C620_PID_RPM.deprev[count] = C620_PID_RPM.decurr[count];
		C620_PID_RPM.decurr[count] = C620.ctrl.RPM[count] - C620.fdbk.RPM[count];

		C620_PID_angle.deprev[count] = C620_PID_angle.decurr[count];
		C620_PID_angle.decurr[count] = C620.ctrl.angle[count] - C620.fdbk.angle[count];
	}
}
#elif defined FDCAN_SUPPORT
#endif