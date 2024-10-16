#include "FDCAN.h"
#include "stm32g4xx_hal.h"
#include "C610.h"

#define ABS(X) ((X) >= 0 ? (X) : -(X))                // 输出X绝对值
#define ABS_LIMIT(X, Y) (X >= 0 ? (X = Y) : (X = -Y)) // 将X限制为±Y

void C610_PID_Angle(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);
void C610_SetI(FDCAN_HandleTypeDef *hfdcan, uint32_t ID);
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
int16_t C610_lap[8];
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

    for (int count = 0; count < 8; count++)
    {
        PID_Angle.pterm[count] = C610_CTRL.Angle[count] - (C610_FDBK.Angle[count] + C610_lap[count] * 360);
        PID_Angle.p[count] = PID_Angle.pterm[count] * C610_ANGLE_Kp;

        if (ABS(PID_Angle.pterm[count]) >= C610_ANGLE_iSTART) // 积分分离
        {
            PID_Angle.iterm[count] = 0;
        }
        else if (ABS(PID_Angle.iterm[count]) <= C610_ANGLE_iLIMIT) // 积分限幅
        {
            PID_Angle.iterm[count] += PID_Angle.pterm[count] * time;
            if (ABS(PID_Angle.iterm[count]) > C610_ANGLE_iLIMIT)
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

    for (int count = 0; count < 8; count++)
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
            if (ABS(PID_RPM.iterm[count]) > C610_RPM_iLIMIT)
                ABS_LIMIT(PID_RPM.iterm[count], C610_RPM_iLIMIT);
        }
        PID_RPM.i[count] = PID_RPM.iterm[count] * C610_RPM_Ki;

        PID_RPM.dterm[count] = (PID_RPM.decurr[count] - PID_RPM.deprev[count]) / time; // 微分先行
        PID_RPM.d[count] = PID_RPM.dterm[count] * C610_RPM_Kd;

        C610_CTRL.I[count] = PID_RPM.p[count] + PID_RPM.i[count] + PID_RPM.d[count];
    }
    C610_SetI(hfdcan, ID);
}

void C610_SetI(FDCAN_HandleTypeDef *hfdcan, uint32_t ID)
{
    uint8_t C610[16];

    for (int count = 0; count < 8; count++)
    {
        if (ABS(C610_CTRL.I[count]) > C610_I_MAX)
            ABS_LIMIT(C610_CTRL.I[count], C610_I_MAX);
        C610[count * 2] = (int16_t)(C610_CTRL.I[count] / C610_fI) >> 8;
        C610[count * 2 + 1] = (int16_t)(C610_CTRL.I[count] / C610_fI);
    }

    FDCAN_SendData(hfdcan, ID, ID == C610_ID1 ? C610 : &C610[8], 8);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
    uint8_t RxFifo0[8];
    if (hfdcan->Instance == FDCAN1)
    {
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, RxFifo0);
        uint8_t count = FDCAN_RxHeader.Identifier - 0x200 - 1;

        C610_FDBK.Angle[count] = (int16_t)((RxFifo0[0] << 8) | RxFifo0[1]) * C610_fANGLE;
        C610_FDBK.RPM[count] = (int16_t)((RxFifo0[2] << 8) | RxFifo0[3]);
        C610_FDBK.T[count] = (int16_t)((RxFifo0[4] << 8) | RxFifo0[5]);

        static int16_t angle_prev[8];

        if (ABS(angle_prev[count] - C610_FDBK.Angle[count]) >= C610_DPS_MAX) // 计圈
            angle_prev[count] > C610_FDBK.Angle[count] ? C610_lap[count]++ : C610_lap[count]--;
        angle_prev[count] = C610_FDBK.Angle[count];

        PID_RPM.deprev[count] = PID_RPM.decurr[count];
        PID_RPM.decurr[count] = C610_CTRL.RPM[count] - C610_FDBK.RPM[count];

#ifdef C610_MODE_ANGLE
        PID_Angle.deprev[count] = PID_Angle.decurr[count];
        PID_Angle.decurr[count] = C610_CTRL.Angle[count] - (C610_FDBK.Angle[count] + C610_lap[count] * 360);
#endif
    }
}