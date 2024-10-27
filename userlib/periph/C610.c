#include "FDCAN.h"
#include "stm32g4xx_hal.h"
#include "C610.h"

#define ABS(X) ((X) >= 0 ? (X) : -(X)) // 输出X绝对值
#define ABS_LIMIT(X, Y) \
    if (ABS(X) > Y)     \
    X >= 0 ? (X = Y) : (X = -Y)

void C610_PID_Angle(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);
void C610_SetCurrent(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);
void C610_PID_RPM(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);

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
struct C610 C610_CTRL, C610_FDBK;
static float time;

void C610_PID_Angle(FDCAN_HandleTypeDef *hfdcan, uint32_t ID)
{
#ifdef C610_MODE_ANGLE
    static float time_prev, time_curr;
    time_curr = (float)(TIM6->CNT + 1) / (TIM6->ARR + 1);
    if (time_prev > time_curr) // 时间间隔
        time = 1 + time_curr - time_prev;
    else
        time = time_curr - time_prev;

    time_prev = time_curr;
#endif

    for (int count = (ID == C610_ID2 ? 4 : 0); count < (ID == C610_ID1 ? 4 : 8); count++)
    {
        PID_Angle.pterm[count] = C610_CTRL.Angle[count] - C610_FDBK.Angle[count];
        PID_Angle.p[count] = PID_Angle.pterm[count] * C610_ANGLE_Kp;

        if (ABS(PID_Angle.pterm[count]) >= C610_ANGLE_iSTART) // 积分分离
        {
            PID_Angle.iterm[count] = 0;
        }
        else if (ABS(PID_Angle.iterm[count]) <= C610_ANGLE_iLIMIT) // 积分限幅
        {
            PID_Angle.iterm[count] += PID_Angle.pterm[count] * time;
            ABS_LIMIT(PID_Angle.iterm[count], C610_ANGLE_iLIMIT);
        }
        PID_Angle.i[count] = PID_Angle.iterm[count] * C610_ANGLE_Ki;

        PID_Angle.dterm[count] = (PID_Angle.decurr[count] - PID_Angle.deprev[count]) / time; // 微分先行
        PID_Angle.d[count] = PID_Angle.dterm[count] * C610_ANGLE_Kd;

        C610_CTRL.RPM[count] = PID_Angle.p[count] + PID_Angle.i[count] + PID_Angle.d[count];
    }
    C610_PID_RPM(hfdcan, ID);
}

void C610_PID_RPM(FDCAN_HandleTypeDef *hfdcan, uint32_t ID)
{
#ifdef C610_MODE_RPM
    static float time_prev, time_curr;
    time_curr = (float)(TIM6->CNT + 1) / (TIM6->ARR + 1);
    if (time_prev > time_curr) // 时间间隔
        time = 1 + time_curr - time_prev;
    else
        time = time_curr - time_prev;

    time_prev = time_curr;
#endif

    for (int count = (ID == C610_ID2 ? 4 : 0); count < (ID == C610_ID1 ? 4 : 8); count++)
    {
        PID_RPM.pterm[count] = C610_CTRL.RPM[count] - C610_FDBK.RPM[count];
        PID_RPM.p[count] = PID_RPM.pterm[count] * C610_RPM_Kp;

        if (ABS(PID_RPM.pterm[count]) >= C610_RPM_iSTART) // 积分分离
        {
            PID_RPM.iterm[count] *= 0.4;
        }
        else if (ABS(PID_RPM.iterm[count]) <= C610_RPM_iLIMIT) // 积分限幅
        {
            PID_RPM.iterm[count] += PID_RPM.pterm[count] * time;
            ABS_LIMIT(PID_RPM.iterm[count], C610_RPM_iLIMIT);
        }
        PID_RPM.i[count] = PID_RPM.iterm[count] * C610_RPM_Ki;

        PID_RPM.dterm[count] = (PID_RPM.decurr[count] - PID_RPM.deprev[count]) / time; // 微分先行
        PID_RPM.d[count] = PID_RPM.dterm[count] * C610_RPM_Kd;

        C610_CTRL.Current[count] = PID_RPM.p[count] + PID_RPM.i[count] + PID_RPM.d[count];
    }
    C610_SetCurrent(hfdcan, ID);
}

void C610_SetTorque(FDCAN_HandleTypeDef *hfdcan, uint32_t ID)
{
    for (int count = (ID == C610_ID2 ? 4 : 0); count < (ID == C610_ID1 ? 4 : 8); count++)
    {
        C610_CTRL.Current[count] = C610_CTRL.Torque[count] / C610_fTORQUE;
    }
    C610_SetCurrent(hfdcan, ID);
}

void C610_SetCurrent(FDCAN_HandleTypeDef *hfdcan, uint32_t ID)
{
    uint8_t C610[16];

    for (int count = (ID == C610_ID2 ? 4 : 0); count < (ID == C610_ID1 ? 4 : 8); count++)
    {
        ABS_LIMIT(C610_CTRL.Current[count], C610_CURRENT_MAX);
        C610[count * 2] = (int16_t)(C610_CTRL.Current[count] / C610_fCURRENT) >> 8;
        C610[count * 2 + 1] = (int16_t)(C610_CTRL.Current[count] / C610_fCURRENT);
    }
    
    if (ID == (C610_ID1 | C610_ID2))
    {
        FDCAN_SendData(hfdcan, FDCAN_STANDARD_ID, C610_ID1, C610, 8);
        FDCAN_SendData(hfdcan, FDCAN_STANDARD_ID, C610_ID2, &C610[8], 8);
    }
    else
        FDCAN_SendData(hfdcan, FDCAN_STANDARD_ID, ID, ID == C610_ID1 ? C610 : &C610[8], 8);
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
    FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
    uint8_t RxFifo1[8];
    if (hfdcan->Instance == FDCAN2)
    {
        static struct // add delay at initialization to set sw zero point
        {
            uint8_t ZP_Status[8];
            float ZP[8];
        } C610_ZP;
        static int16_t C610_lap[8];

        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &FDCAN_RxHeader, RxFifo1);
        uint8_t count = FDCAN_RxHeader.Identifier - 0x200 - 1;

        static float angle_prev[8], angle_curr[8];
        if (!C610_ZP.ZP_Status[count])
        {
            angle_prev[count] = angle_curr[count] = C610_ZP.ZP[count] = ((RxFifo1[0] << 8) | RxFifo1[1]) * C610_fANGLE / C610_GR;
            C610_ZP.ZP_Status[count] = 1;
        }
        else
        {
            angle_curr[count] = ((RxFifo1[0] << 8) | RxFifo1[1]) * C610_fANGLE / C610_GR;
            if (ABS(angle_prev[count] - angle_curr[count]) >= C610_DPS_MAX) // 计圈
                angle_prev[count] > angle_curr[count] ? C610_lap[count]++ : C610_lap[count]--;
            angle_prev[count] = angle_curr[count];
        }

        C610_FDBK.Angle[count] = angle_curr[count] + C610_lap[count] * 360 / C610_GR - C610_ZP.ZP[count];
        C610_FDBK.RPM[count] = (int16_t)((RxFifo1[2] << 8) | RxFifo1[3]) / C610_GR;
        C610_FDBK.Torque[count] = (int16_t)((RxFifo1[4] << 8) | RxFifo1[5]) * C610_fCURRENT * C610_fTORQUE;

        PID_RPM.deprev[count] = PID_RPM.decurr[count];
        PID_RPM.decurr[count] = C610_CTRL.RPM[count] - C610_FDBK.RPM[count];

#ifdef C610_MODE_ANGLE
        PID_Angle.deprev[count] = PID_Angle.decurr[count];
        PID_Angle.decurr[count] = C610_CTRL.Angle[count] - C610_FDBK.Angle[count];
#endif
    }
}