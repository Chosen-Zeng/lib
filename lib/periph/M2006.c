#include "M2006.h"
#include "TIM.h"
#include "algorithm.h"

#ifdef CAN_SUPPORT
#include "CAN.h"
#elif defined FDCAN_SUPPORT
#include "FDCAN.h"
#endif

#ifdef FREQ

C610_PID_t C610_PID_RPM, C610_PID_angle;

C610_t C610;

void C610_SetCurrent(void *CAN_handle, uint32_t C610_ID)
{
    uint8_t TxData[16];

    for (int count = (C610_ID == C610_ID2 ? 4 : 0); count < (C610_ID == C610_ID1 ? 4 : 8); count++)
    {
        LIMIT_ABS(C610.ctrl.current[count], C610_CURRENT_LIMIT)
        TxData[count * 2] = (int16_t)(C610.ctrl.current[count] / C610_fCURRENT) >> 8;
        TxData[count * 2 + 1] = (int16_t)(C610.ctrl.current[count] / C610_fCURRENT);
    }

    if (C610_ID == (C610_ID1 | C610_ID2))
    {
#ifdef CAN_SUPPORT
        CAN_SendData(CAN_handle, CAN_ID_STD, C610_ID1, TxData, 8);
        CAN_SendData(CAN_handle, CAN_ID_STD, C610_ID2, &TxData[8], 8);
#elif defined FDCAN_SUPPORT
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C610_ID1, TxData, 8);
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C610_ID2, &TxData[8], 8);
#endif
    }
    else
#ifdef CAN_SUPPORT
        CAN_SendData(CAN_handle, CAN_ID_STD, C610_ID, C610_ID == C610_ID1 ? TxData : &TxData[8], 8);
#elif defined FDCAN_SUPPORT
        CAN_SendData(CAN_handle, FDCAN_STANDARD_ID, C610_ID, C610_ID == C610_ID1 ? TxData : &TxData[8], 8);
#endif
}

void C610_SetRPM(void *CAN_handle, uint32_t C610_ID)
{
    for (int count = (C610_ID == C610_ID2 ? 4 : 0); count < (C610_ID == C610_ID1 ? 4 : 8); count++)
    {
        C610_PID_RPM.pterm[count] = C610.ctrl.RPM[count] - C610.fdbk.RPM[count];
        C610_PID_RPM.p[count] = C610_PID_RPM.pterm[count] * C610_RPM_Kp;

        if (ABS(C610_PID_RPM.pterm[count]) >= C610_RPM_iSTART) // 积分分离
        {
            C610_PID_RPM.iterm[count] *= 0.4;
        }
        else
        {
            C610_PID_RPM.iterm[count] += C610_PID_RPM.pterm[count] / FREQ;
            LIMIT_ABS(C610_PID_RPM.iterm[count], C610_RPM_iLIMIT); // 积分限幅
        }
        C610_PID_RPM.i[count] = C610_PID_RPM.iterm[count] * C610_RPM_Ki;

        C610_PID_RPM.dterm[count] = (C610_PID_RPM.decurr[count] - C610_PID_RPM.deprev[count]) * FREQ; // 微分先行
        C610_PID_RPM.d[count] = C610_PID_RPM.dterm[count] * C610_RPM_Kd;

        C610.ctrl.current[count] = C610_PID_RPM.p[count] + C610_PID_RPM.i[count] + C610_PID_RPM.d[count];
    }
    C610_SetCurrent(CAN_handle, C610_ID);
}

void C610_SetAngle(void *CAN_handle, uint32_t C610_ID)
{

    for (int count = (C610_ID == C610_ID2 ? 4 : 0); count < (C610_ID == C610_ID1 ? 4 : 8); count++)
    {
        C610_PID_angle.pterm[count] = C610.ctrl.angle[count] - C610.fdbk.angle[count];
        C610_PID_angle.p[count] = C610_PID_angle.pterm[count] * C610_ANGLE_Kp;

        if (ABS(C610_PID_angle.pterm[count]) >= C610_ANGLE_iSTART) // 积分分离
        {
            C610_PID_angle.iterm[count] = 0;
        }
        else if (ABS(C610_PID_angle.iterm[count]) <= C610_ANGLE_iLIMIT) // 积分限幅
        {
            C610_PID_angle.iterm[count] += C610_PID_angle.pterm[count] / FREQ;
            LIMIT_ABS(C610_PID_angle.iterm[count], C610_ANGLE_iLIMIT);
        }
        C610_PID_angle.i[count] = C610_PID_angle.iterm[count] * C610_ANGLE_Ki;

        C610_PID_angle.dterm[count] = (C610_PID_angle.decurr[count] - C610_PID_angle.deprev[count]) * FREQ; // 微分先行
        C610_PID_angle.d[count] = C610_PID_angle.dterm[count] * C610_ANGLE_Kd;

        C610.ctrl.RPM[count] = C610_PID_angle.p[count] + C610_PID_angle.i[count] + C610_PID_angle.d[count];
    }
    C610_SetRPM(CAN_handle, C610_ID);
}

void C610_SetTorque(void *CAN_handle, uint32_t C610_ID)
{
    for (int count = (C610_ID == C610_ID2 ? 4 : 0); count < (C610_ID == C610_ID1 ? 4 : 8); count++)
    {
        C610.ctrl.current[count] = C610.ctrl.torque[count] / M2006_fTORQUE;
    }
    C610_SetCurrent(CAN_handle, C610_ID);
}

#ifdef CAN_SUPPORT
#elif defined FDCAN_SUPPORT
__weak void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
    FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
    uint8_t RxFifo1[8];
    if (hfdcan->Instance == FDCAN2)
    {
        // sw reset unusable
        /*
        static struct // add delay at initialization to set sw zero point
        {
            uint8_t ZP_Status[8];
            float ZP[8];
        } C610_ZP;
        */

        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &FDCAN_RxHeader, RxFifo1);
        uint8_t count = FDCAN_RxHeader.Identifier - 0x200 - 1;

        static float angle_prev[8], angle_curr[8];
        /*
        if (!C610_ZP.ZP_Status[count])
        {
            angle_prev[count] = angle_curr[count] = C610_ZP.ZP[count] = ((RxFifo1[0] << 8) | RxFifo1[1]) * C610_fANGLE / C610_GR;
            C610_ZP.ZP_Status[count] = 1;
        }
        else
        */
        {
            angle_curr[count] = ((RxFifo1[0] << 8) | RxFifo1[1]) * C610_fANGLE;
            if (ABS(angle_prev[count] - angle_curr[count]) >= 180) // 计圈
                C610.fdbk.angle[count] += ((angle_prev[count] > angle_curr[count] ? 360 : -360) + angle_curr[count] - angle_prev[count]) / M2006_GR;
            else
                C610.fdbk.angle[count] += (angle_curr[count] - angle_prev[count]) / M2006_GR;
            angle_prev[count] = angle_curr[count];
        }

        C610.fdbk.RPM[count] = (int16_t)((RxFifo1[2] << 8) | RxFifo1[3]) / M2006_GR;
        C610.fdbk.current[count] = (int16_t)((RxFifo1[4] << 8) | RxFifo1[5]) * C610_fCURRENT;
        C610.fdbk.torque[count] = C610.fdbk.current[count] * M2006_fTORQUE;

        C610_PID_RPM.deprev[count] = C610_PID_RPM.decurr[count];
        C610_PID_RPM.decurr[count] = C610.ctrl.RPM[count] - C610.fdbk.RPM[count];

        C610_PID_angle.deprev[count] = C610_PID_angle.decurr[count];
        C610_PID_angle.decurr[count] = C610.ctrl.angle[count] - C610.fdbk.angle[count];
    }
}
#endif
#endif