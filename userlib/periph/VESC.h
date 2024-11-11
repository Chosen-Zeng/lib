#ifndef __VESC_H
#define __VESC_H

#define T_MOTOR_AT4130_KV450

#ifdef T_MOTOR_AT4130_KV450
#define VESC_MOTOR_CURRENT_MAX 75
#define VESC_MOTOR_RPM_MAX 7200 // unexact
#define VESC_MOTOR_PP (14 / 2)
#else
#error No motor info.
#endif

#define VESC_MAX 4

#define VESC_fPCT_W 100000
#define VESC_fCURRENT_W 1000
// pos

#define VESC_fPCT_R 1000
#define VESC_fCURRENT_R 10
// pos

typedef struct
{
    float RPM;
    float DutyCycle;
    float Temp_FET;
    float Temp_Motor;
    float Current_In;
    float PID_Pos;
} VESC_FDBK_t;

extern float VESC_RPM[VESC_MAX],
    VESC_DutyCycle[VESC_MAX]; // VESC_Current[VESC_MAX],

void VESC_SendCMD(void *CAN_handle, uint8_t ID, uint8_t VESC_Command, float data);

#endif