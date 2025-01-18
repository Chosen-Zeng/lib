#include "M3508.h"
#include "TIM.h"
#include "algorithm.h"

#ifdef CAN_SUPPORT
#include "CAN.h"
#elif defined FDCAN_SUPPORT
#include "FDCAN.h"
#endif

#ifdef FREQ

C620_t C620[8];

C620_PID_t C620_PID_spd[8], C620_PID_pos[8];

void C620_SetCurrent(void *CAN_handle, uint32_t C620_ID)
{
	uint8_t TxData[16];

	for (int ID_array = (C620_ID == C620_ID2 ? 4 : 0); ID_array < (C620_ID == C620_ID1 ? 4 : 8); ID_array++)
	{
		LIMIT_ABS(C620[ID_array].ctrl.current, C620_CURRENT_LIMIT)
		TxData[ID_array * 2] = (int16_t)(C620[ID_array].ctrl.current / C620_fCURRENT) >> 8;
		TxData[ID_array * 2 + 1] = (int16_t)(C620[ID_array].ctrl.current / C620_fCURRENT);
	}

	if (C620_ID == (C620_ID1 | C620_ID2))
	{
#ifdef CAN_SUPPORT
		CAN_SendData(CAN_handle, CAN_ID_STD, C620_ID1, TxData, 8);
		CAN_SendData(CAN_handle, CAN_ID_STD, C620_ID2, &TxData[8], 8);
#elif defined FDCAN_SUPPORT
		CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C620_ID1, TxData, 8);
		CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C620_ID2, &TxData[8], 8);
#endif
	}
	else
#ifdef CAN_SUPPORT
		CAN_SendData(CAN_handle, CAN_ID_STD, C620_ID, C620_ID == C620_ID1 ? TxData : &TxData[8], 8);
#elif defined FDCAN_SUPPORT
		CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C620_ID, C620_ID == C620_ID1 ? TxData : &TxData[8], 8);
#endif
}

void C620_SetSpd(void *CAN_handle, uint32_t C620_ID)
{

	for (int ID_array = (C620_ID == C620_ID2 ? 4 : 0); ID_array < (C620_ID == C620_ID1 ? 4 : 8); ID_array++)
	{
		C620_PID_spd[ID_array].pterm = C620[ID_array].ctrl.spd - C620[ID_array].fdbk.spd;
		C620_PID_spd[ID_array].p = C620_PID_spd[ID_array].pterm * C620_SPD_Kp;

		if (ABS(C620_PID_spd[ID_array].pterm) >= C620_SPD_iSTART) // 积分分离
		{
			C620_PID_spd[ID_array].iterm *= 0.4;
		}
		else
		{
			C620_PID_spd[ID_array].iterm += C620_PID_spd[ID_array].pterm / FREQ;
			LIMIT_ABS(C620_PID_spd[ID_array].iterm, C620_SPD_iLIMIT); // 积分限幅
		}
		C620_PID_spd[ID_array].i = C620_PID_spd[ID_array].iterm * C620_SPD_Ki;

		C620_PID_spd[ID_array].dterm = (C620_PID_spd[ID_array].decurr - C620_PID_spd[ID_array].deprev) * FREQ; // 微分先行
		C620_PID_spd[ID_array].d = C620_PID_spd[ID_array].dterm * C620_SPD_Kd;

		C620[ID_array].ctrl.current = C620_PID_spd[ID_array].p + C620_PID_spd[ID_array].i + C620_PID_spd[ID_array].d;
	}
	C620_SetCurrent(CAN_handle, C620_ID);
}

void C620_SetPos(void *CAN_handle, uint32_t C620_ID)
{

	for (int ID_array = (C620_ID == C620_ID2 ? 4 : 0); ID_array < (C620_ID == C620_ID1 ? 4 : 8); ID_array++)
	{
		C620_PID_pos[ID_array].pterm = C620[ID_array].ctrl.pos - C620[ID_array].fdbk.pos;
		C620_PID_pos[ID_array].p = C620_PID_pos[ID_array].pterm * C620_POS_Kp;

		if (ABS(C620_PID_pos[ID_array].pterm) >= C620_POS_iSTART) // 积分分离
		{
			C620_PID_pos[ID_array].iterm = 0;
		}
		else if (ABS(C620_PID_pos[ID_array].iterm) <= C620_POS_iLIMIT) // 积分限幅
		{
			C620_PID_pos[ID_array].iterm += C620_PID_pos[ID_array].pterm / FREQ;
			LIMIT_ABS(C620_PID_pos[ID_array].iterm, C620_POS_iLIMIT);
		}
		C620_PID_pos[ID_array].i = C620_PID_pos[ID_array].iterm * C620_POS_Ki;

		C620_PID_pos[ID_array].dterm = (C620_PID_pos[ID_array].decurr - C620_PID_pos[ID_array].deprev) * FREQ; // 微分先行
		C620_PID_pos[ID_array].d = C620_PID_pos[ID_array].dterm * C620_POS_Kd;

		C620[ID_array].ctrl.spd = C620_PID_pos[ID_array].p + C620_PID_pos[ID_array].i + C620_PID_pos[ID_array].d;
	}
	C620_SetSpd(CAN_handle, C620_ID);
}

void C620_SetTorque(void *CAN_handle, uint32_t C620_ID)
{
	for (int ID_array = (C620_ID == C620_ID2 ? 4 : 0); ID_array < (C620_ID == C620_ID1 ? 4 : 8); ID_array++)
	{
		C620[ID_array].ctrl.current = C620[ID_array].ctrl.torque / M3508_fTORQUE;
	}
	C620_SetCurrent(CAN_handle, C620_ID);
}

#ifdef CAN_SUPPORT
__weak void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef CAN_RxHeader;
	uint8_t RxFifo0[8];
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_RxHeader, RxFifo0);

	if (hcan->Instance == CAN1)
	{
		uint8_t ID_array = CAN_RxHeader.StdId - 0x200 - 1;
		static float pos_prev[8], pos_curr[8];

		pos_curr[ID_array] = ((RxFifo0[0] << 8) | RxFifo0[1]) * C620_fPOS;
		if (ABS(pos_prev[ID_array] - pos_curr[ID_array]) >= 180) // lap
			C620[ID_array].fdbk.pos += ((pos_prev[ID_array] > pos_curr[ID_array] ? 360 : -360) - pos_prev[ID_array] + pos_curr[ID_array]) / M3508_GR;
		else
			C620[ID_array].fdbk.pos += (pos_curr[ID_array] - pos_prev[ID_array]) / M3508_GR;
		pos_prev[ID_array] = pos_curr[ID_array];

		C620[ID_array].fdbk.spd = (int16_t)((RxFifo0[2] << 8) | RxFifo0[3]) / M3508_GR;
		C620[ID_array].fdbk.current = (int16_t)((RxFifo0[4] << 8) | RxFifo0[5]) * C620_fCURRENT;
		C620[ID_array].fdbk.torque = C620[ID_array].fdbk.current * M3508_fTORQUE;
		C620[ID_array].fdbk.temp = RxFifo0[6];

		C620_PID_spd[ID_array].deprev = C620_PID_spd[ID_array].decurr;
		C620_PID_spd[ID_array].decurr = C620[ID_array].ctrl.spd - C620[ID_array].fdbk.spd;

		C620_PID_pos[ID_array].deprev = C620_PID_pos[ID_array].decurr;
		C620_PID_pos[ID_array].decurr = C620[ID_array].ctrl.pos - C620[ID_array].fdbk.pos;
	}
}
#elif defined FDCAN_SUPPORT
#endif
#endif