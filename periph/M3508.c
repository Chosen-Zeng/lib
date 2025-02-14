#include "M3508.h"
#include "TIM.h"
#include "algorithm.h"
#include "CAN.h"

#ifdef FREQ_CTRL

C620_t C620[8];

C620_PID_t C620_PID_spd[8], C620_PID_pos[8];

void C620_SetCurrent(void *CAN_handle, unsigned short C620_ID)
{
	unsigned char TxData[16];

	for (unsigned char ID_array = (C620_ID == C620_ID2 ? 4 : 0); ID_array < (C620_ID == C620_ID1 ? 4 : 8); ID_array++)
	{
		TxData[ID_array * 2] = (short)(LIMIT_ABS(C620[ID_array].ctrl.curr, C620_CURR_LIMIT) / C620_fCURR) >> 8;
		TxData[ID_array * 2 + 1] = (short)(C620[ID_array].ctrl.curr / C620_fCURR);
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

void C620_SetSpd(void *CAN_handle, unsigned short C620_ID)
{
	for (unsigned char ID_array = (C620_ID == C620_ID2 ? 4 : 0); ID_array < (C620_ID == C620_ID1 ? 4 : 8); ID_array++)
	{
		C620_PID_spd[ID_array].pterm = C620[ID_array].ctrl.spd - C620[ID_array].fdbk.spd;
		C620_PID_spd[ID_array].p = C620_PID_spd[ID_array].pterm * C620_SPD_Kp;

		if (ABS(C620_PID_spd[ID_array].pterm) >= C620_SPD_iSTART) // 积分分离
			C620_PID_spd[ID_array].iterm *= 0.4;
		else
			C620_PID_spd[ID_array].iterm += C620_PID_spd[ID_array].pterm / FREQ_CTRL;

		C620_PID_spd[ID_array].i = LIMIT_ABS(C620_PID_spd[ID_array].iterm, C620_SPD_iLIMIT) * C620_SPD_Ki; // 积分限幅

		C620_PID_spd[ID_array].dterm = (C620_PID_spd[ID_array].decurr - C620_PID_spd[ID_array].deprev) * FREQ_CTRL; // 微分先行
		C620_PID_spd[ID_array].d = C620_PID_spd[ID_array].dterm * C620_SPD_Kd;

		C620[ID_array].ctrl.curr = C620_PID_spd[ID_array].p + C620_PID_spd[ID_array].i + C620_PID_spd[ID_array].d;
	}
	C620_SetCurrent(CAN_handle, C620_ID);
}

void C620_SetPos(void *CAN_handle, unsigned short C620_ID)
{
	for (unsigned char ID_array = (C620_ID == C620_ID2 ? 4 : 0); ID_array < (C620_ID == C620_ID1 ? 4 : 8); ID_array++)
	{
		C620_PID_pos[ID_array].pterm = C620[ID_array].ctrl.pos - C620[ID_array].fdbk.pos;
		C620_PID_pos[ID_array].p = C620_PID_pos[ID_array].pterm * C620_POS_Kp;

		if (ABS(C620_PID_pos[ID_array].pterm) >= C620_POS_iSTART) // 积分分离
			C620_PID_pos[ID_array].iterm = 0;
		else if (ABS(C620_PID_pos[ID_array].iterm) <= C620_POS_iLIMIT)
			C620_PID_pos[ID_array].iterm += C620_PID_pos[ID_array].pterm / FREQ_CTRL;

		C620_PID_pos[ID_array].i = LIMIT_ABS(C620_PID_pos[ID_array].iterm, C620_POS_iLIMIT) * C620_POS_Ki; // 积分限幅

		C620_PID_pos[ID_array].dterm = (C620_PID_pos[ID_array].decurr - C620_PID_pos[ID_array].deprev) * FREQ_CTRL; // 微分先行
		C620_PID_pos[ID_array].d = C620_PID_pos[ID_array].dterm * C620_POS_Kd;

		C620[ID_array].ctrl.spd = C620_PID_pos[ID_array].p + C620_PID_pos[ID_array].i + C620_PID_pos[ID_array].d;
	}
	C620_SetSpd(CAN_handle, C620_ID);
}

void C620_SetTorque(void *CAN_handle, unsigned short C620_ID)
{
	for (unsigned char ID_array = (C620_ID == C620_ID2 ? 4 : 0); ID_array < (C620_ID == C620_ID1 ? 4 : 8); ID_array++)
		C620[ID_array].ctrl.curr = C620[ID_array].ctrl.trq / M3508_fTRQ;

	C620_SetCurrent(CAN_handle, C620_ID);
}

#ifdef CAN_SUPPORT
__weak void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef CAN_RxHeader;
	unsigned char RxFifo0[8];
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_RxHeader, RxFifo0);

	if (hcan->Instance == CAN1)
	{
		unsigned char ID_array = CAN_RxHeader.StdId - 0x200 - 1;
		static float pos_prev[8], pos_curr[8];

		pos_curr[ID_array] = ((RxFifo0[0] << 8) | RxFifo0[1]) * C620_fPOS;
		if (ABS(pos_prev[ID_array] - pos_curr[ID_array]) >= 180) // lap
			C620[ID_array].fdbk.pos += ((pos_prev[ID_array] > pos_curr[ID_array] ? 360 : -360) - pos_prev[ID_array] + pos_curr[ID_array]) / M3508_GR;
		else
			C620[ID_array].fdbk.pos += (pos_curr[ID_array] - pos_prev[ID_array]) / M3508_GR;
		pos_prev[ID_array] = pos_curr[ID_array];

		C620[ID_array].fdbk.spd = (short)((RxFifo0[2] << 8) | RxFifo0[3]) / M3508_GR;
		C620[ID_array].fdbk.curr = (short)((RxFifo0[4] << 8) | RxFifo0[5]) * C620_fCURR;
		C620[ID_array].fdbk.trq = C620[ID_array].fdbk.curr * M3508_fTRQ;
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