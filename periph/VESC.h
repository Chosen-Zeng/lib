#ifndef __VESC_H
#define __VESC_H

#include "user.h"

#if defined VESC_NUM && defined VESC_ID_OFFSET

typedef const struct
{
    const float curr_max;
    const unsigned short spd_max; // speed at 24V
    const unsigned char PP;
} motor_info_t;
extern motor_info_t T_MOTOR_AT4130_KV450,
    HOBBYWING_V9626_KV160,
    CUBEMARS_R100_KV90,
    CUBEMARS_RO100_KV55;

#define VESC_fPCT_W 100000
#define VESC_fCURR_W 1000
#define VESC_fPOS_W 1000000

#define VESC_fPCT_R 1000
#define VESC_fCURR_R 10
#define VESC_fPOS_R 50

#define VESC_fVOLT 10

#define VESC_SET_DUTYCYCLE 0
#define VESC_SET_CURR 1
#define VESC_SET_CURR_BRAKE 2
#define VESC_SET_SPD 3
#define VESC_SET_POS 4
#define VESC_SET_CURR_REL 10
#define VESC_SET_STOP_REL 11
#define VESC_SET_CURR_HANDBRAKE 12
#define VESC_SET_CURR_HANDBRAKE_REL 13

#define VESC_POS_MAX 359

#define VESC_STATUS_1 9
#define VESC_STATUS_2 14
#define VESC_STATUS_3 15
#define VESC_STATUS_4 16
#define VESC_STATUS_5 27
#define VESC_STATUS_6 28

typedef struct
{
    struct
    {
        float curr, spd, pos;
    } ctrl;
    struct
    {
        // CAN_PACKET_STATUS_1
        float spd, curr, dutycycle;

        // CAN_PACKET_STATUS_4
        struct
        {
            float MOSFET, motor;
        } temp;
        float curr_in, pos;

        // CAN_PACKET_STATUS_5
        float tachometer, volt;
    } fdbk;
} VESC_t;
extern VESC_t VESC[VESC_NUM];

void VESC_SendCmd(void *CAN_handle, unsigned char ID, unsigned short VESC_cmd, motor_info_t *motor_info);

#endif
#endif