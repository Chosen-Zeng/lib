#ifndef __VESC_H
#define __VESC_H

#define T_MOTOR_AT4130_KV450

#ifdef T_MOTOR_AT4130_KV450
#define VESC_MOTOR_CURRENT_MAX 75
#define VESC_MOTOR_RPM_MAX 7200  //unexact
#define VESC_MOTOR_PP 14
#endif

#define VESC_MAX 4

#define VESC_fPCT_W 100000
#define VESC_fCURRENT_W 1000
//pos

#define VESC_fPCT_R 1000
#define VESC_fCURRENT_R 10
//pos

void VESC_SendData(FDCAN_HandleTypeDef *hfdcan, uint8_t VESC_ID, uint8_t VESC_Command, float data);

#endif