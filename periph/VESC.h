#ifndef __VESC_H
#define __VESC_H

#include "user.h"

#if defined VESC_NUM && defined VESC_ID_OFFSET

#ifdef T_MOTOR_AT4130_KV450
#define VESC_MOTOR_CURR_MAX 75
#define VESC_MOTOR_SPD_MAX 9000
#define VESC_MOTOR_PP 7
#elif defined HOBBYWING_V9626_KV160
#define VESC_MOTOR_CURR_MAX 171.5
#define VESC_MOTOR_SPD_MAX 6000
#define VESC_MOTOR_PP 21
#elif defined CUBEMARS_R100_KV90
#define VESC_MOTOR_CURR_MAX 104
#define VESC_MOTOR_SPD_MAX 3800
#define VESC_MOTOR_PP 21
#endif

#define VESC_fPCT_W 100000
#define VESC_fCURR_W 1000
#define VESC_fPOS_W 1000000

#define VESC_fPCT_R 1000
#define VESC_fCURR_R 10
#define VESC_fPOS_R 50

#define VESC_SET_DUTYCYCLE (0 << 8)
#define VESC_SET_CURR (1 << 8)
#define VESC_SET_CURR_BRAKE (2 << 8)
#define VESC_SET_SPD (3 << 8)
#define VESC_SET_POS (4 << 8)
#define VESC_SET_CURR_REL (10 << 8)
#define VESC_SET_STOP_REL (11 << 8)
#define VESC_SET_CURR_HANDBRAKE (12 << 8)
#define VESC_SET_CURR_HANDBRAKE_REL (13 << 8)

#define VESC_POS_MAX 359

#define VESC_STATUS_1 (9 << 8)
#define VESC_STATUS_2 (14 << 8)
#define VESC_STATUS_3 (15 << 8)
#define VESC_STATUS_4 (16 << 8)
#define VESC_STATUS_5 (27 << 8)
#define VESC_STATUS_6 (28 << 8)

typedef struct
{
    float curr;
    float spd;
    float pos;
} VESC_ctrl_t;

typedef struct
{
    // CAN_PACKET_STATUS
    float spd;
    float dutycycle;

    // CAN_PACKET_STATUS_4
    float temp_MOSFET;
    float temp_motor;
    float curr_in;
    float pos;
} VESC_fdbk_t;

typedef struct
{
    VESC_ctrl_t ctrl;
    VESC_fdbk_t fdbk;
} VESC_t;

extern VESC_t VESC[VESC_NUM];

void VESC_SendCmd(void *CAN_handle, unsigned char ID, unsigned short VESC_cmd);

#endif
#endif