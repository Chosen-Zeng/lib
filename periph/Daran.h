#ifndef __DARAN_H
#define __DARAN_H

#include "user.h"

#if defined DARAN_NUM && defined DARAN_ID_OFFSET

#define DARAN_SCALING 0.01f

#define DARAN_ID_BCAST 0

#define DARAN_DATA_TYPE_f 0
#define DARAN_DATA_TYPE_u16 1
#define DARAN_DATA_TYPE_s16 2
#define DARAN_DATA_TYPE_u32 3
#define DARAN_DATA_TYPE_s32 4

#define DARAN_CMD_PARAM_PRESET_4 0x06

#define DARAN_CMD_SYS_CTRL 0x08
#define DARAN_SYS_CMD_CONFIG_SAVE 0x01
#define DARAN_SYS_CMD_REBOOT 0x03
#define DARAN_SYS_CMD_0POS 0x05
#define DARAN_SYS_CMD_STOP 0x06
#define DARAN_SYS_CMD_0POS_TEMP 0x23
#define DARAN_SYS_CMD_GET_DATA 0x24

#define DARAN_SPD_MODE_FDFWD 1
#define DARAN_SPD_MODE_ACCEL 2
#define DARAN_fSPD (1 / DARAN_SCALING / 60)

#define DARAN_TRQ_MODE_DIRECT 1
#define DARAN_TRQ_MODE_ACCEL 6

#define DARAN_CMD_POS_MODE_LIMIT 0x0B
#define DARAN_CMD_PARAM_PRESET_3 0x0C
#define DARAN_CMD_MOTION_AID 0x0D
#define DARAN_CMD_CONFIG_INIT 0x0E
#define DARAN_CMD_POS_MODE_FLTR 0x19
#define DARAN_FLTR_BW_MAX 300
#define DARAN_CMD_POS_MODE_ACCEL 0x1A
#define DARAN_CMD_POS_MODE_FDFWD 0x1B
#define DARAN_CMD_SPD 0x1C
#define DARAN_CMD_TRQ 0x1D
#define DARAN_CMD_PROP_R 0x1E
#define DARAN_CMD_PROP_W 0x1F

#define DARAN_PARAM_VOLT 1                  // data type: f
#define DARAN_PARAM_CURR 2                  // data type: f
#define DARAN_PARAM_CAN_BAUDRATE 21001      // data type: u32
#define DARAN_PARAM_CAN_FDBK_EN 22001       // data type: u32
#define DARAN_PARAM_FW 30004                // data type: u32
#define DARAN_PARAM_CAN_ID 31001            // data type: u32
#define DARAN_PARAM_CAN_FDBK_INTVL_ms 31002 // data type: u32
#define DARAN_PARAM_PRODUCT_MODEL 31003     // data type: u32
#define DARAN_PARAM_POS_LIM_EN 31201        // data type: u32
#define DARAN_PARAM_POS_MIN 31202           // data type: f
#define DARAN_PARAM_POS_MAX 31203           // data type: f
#define DARAN_PARAM_GR 31204                // data type: f
#define DARAN_PARAM_CURR_STALL_LIM 31205    // data type: f
#define DARAN_PARAM_CRASH_DETECT_EN 31206   // data type: u32
#define DARAN_PARAM_CRASH_DETECT_SENS 31207 // data type: f
#define DARAN_PARAM_ENC_MPL_LIM_EN 31208    // data type: u32
#define DARAN_PARAM_AT_POS 32002            // data type: u32
#define DARAN_PARAM_AT_POS_PRCISION 32003   // data type: f
#define DARAN_PARAM_SPD_LIM_EN 32101        // data type: u32
#define DARAN_PARAM_P 32102                 // data type: f
#define DARAN_PARAM_D 32103                 // data type: f
#define DARAN_PARAM_I 32104                 // data type: f
#define DARAN_PARAM_SPD_LIM 32105           // data type: f, motor side
#define DARAN_PARAM_SPD_LIM_REL 32106       // data type: f
#define DARAN_PARAM_INERTIA 32107           // data type: f
#define DARAN_PARAM_FLTR_BW_IN 32108        // data type: f
#define DARAN_PARAM_PP 33101                // data type: s32
#define DARAN_PARAM_PHASE_IND 33102         // data type: f
#define DARAN_PARAM_PHASE_R 33103           // data type: f
#define DARAN_PARAM_TRQ_CONST 33104         // data type: f
#define DARAN_PARAM_CURR_LIM 33105          // data type: f
#define DARAN_PARAM_CURR_MARGIN 33106       // data type: f
#define DARAN_PARAM_TRQ_LIM 33107           // data type: f, motor side
#define DARAN_PARAM_CURR_BD 33108           // data type: f
#define DARAN_PARAM_CURR_Q 33201            // data type: f
#define DARAN_PARAM_CURR_D 33202            // data type: f
//...
#define DARAN_PARAM_TEMP_BOARD 36002         // data type: f
#define DARAN_PARAM_TEMP_PROT_EN_BOARD 36101 // data type: u32
#define DARAN_PARAM_TEMP_FLOOR_BOARD 36102   // data type: f
#define DARAN_PARAM_TEMP_CEILING_BOARD 36103 // data type: f
#define DARAN_PARAM_TEMP_MOTOR 37002         // data type: f
#define DARAN_PARAM_TEMP_PROT_EN_MOTOR 37101 // data type: u32
#define DARAN_PARAM_TEMP_FLOOR_MOTOR 37102   // data type: f
#define DARAN_PARAM_TEMP_CEILING_MOTOR 37103 // data type: f
#define DARAN_PARAM_POS 38001                // data type: f
#define DARAN_PARAM_SPD 38002                // data type: f
#define DARAN_PARAM_TRQ 38003                // data type: f
#define DARAN_PARAM_POS_MIN_TEMP 38004       // data type: f
#define DARAN_PARAM_POS_MAX_TEMP 38005       // data type: f
#define DARAN_PARAM_POS_LIM_TEMP_EN 38006    // data type: u32

typedef struct
{
    struct
    {
        float pos, spd, trq;
    } ctrl, fdbk;
} DaRan_t;
extern DaRan_t DaRan[DARAN_NUM + 1];

void DaRan_CANFdbkInit(void *CAN_handle, unsigned char ID, float intvl_ms);
void DaRan_SetPos(void *CAN_handle, unsigned char ID, unsigned char DARAN_CMD_POS_MODE, float param);
void DaRan_Prop_W(void *CAN_handle, unsigned char ID, unsigned short DARAN_PARAM, unsigned short DARAN_DATA_TYPE, float param_val);
void DaRan_Prop_R(void *CAN_handle, unsigned char ID, unsigned short DARAN_PARAM, unsigned short DARAN_DATA_TYPE);
void DaRan_SetSpd(void *CAN_handle, unsigned char ID, unsigned char DARAN_SPD_MODE, float accel);
void DaRan_SetTrq(void *CAN_handle, unsigned char ID, unsigned char DARAN_TRQ_MODE, float accel);

#endif
#endif