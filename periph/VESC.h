#ifndef __VESC_H
#define __VESC_H

#include "user.h"

#ifdef T_MOTOR_AT4130_KV450
#define VESC_MOTOR_CURRENT_MAX 75
#define VESC_MOTOR_RPM_MAX 9000
#define VESC_MOTOR_PP 7

#elif defined HOBBYWING_V9626_160KV
#define VESC_MOTOR_CURRENT_MAX 171.5
#define VESC_MOTOR_RPM_MAX 6000
#define VESC_MOTOR_PP 21

#endif

#if defined VESC_NUM && defined VESC_ID_OFFSET

#define VESC_fPCT_W 100000
#define VESC_fCURRENT_W 1000

#define VESC_fPCT_R 1000
#define VESC_fCURRENT_R 10

typedef struct
{
    float curr;
    float spd;
} VESC_ctrl_t;

typedef struct
{
    // CAN_PACKET_STATUS
    float spd;
    float DutyCycle;

    // CAN_PACKET_STATUS_4
    float temp_MOSFET;
    float temp_motor;
    float curr_in;
} VESC_fdbk_t;

typedef struct
{
    VESC_ctrl_t ctrl;
    VESC_fdbk_t fdbk;
} VESC_t;

extern VESC_t VESC[VESC_NUM];

void VESC_SendCmd(void *CAN_handle, uint8_t ID, uint8_t VESC_Command);

#endif
#endif