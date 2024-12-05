#ifndef __VESC_H
#define __VESC_H

#define T_MOTOR_AT4130_KV450

#ifdef T_MOTOR_AT4130_KV450
#define VESC_MOTOR_CURRENT_MAX 75
#define VESC_MOTOR_RPM_MAX 9000
#define VESC_MOTOR_PP (14 / 2)
#else
#error No motor info.
#endif

#define VESC_NUM 4
#define VESC_ID_OFFSET 0

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
} VESC_fdbk_t;

extern float VESC_RPM[VESC_NUM],
    VESC_DutyCycle[VESC_NUM]; // VESC_Current[VESC_NUM],

void VESC_SendCmd(void *CAN_handle, uint8_t ID, uint8_t VESC_Command, float data);

#endif