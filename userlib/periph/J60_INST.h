#ifndef __J60_INST
#define __J60_INST

#define J60_MOTOR_DISABLE (0x02 << 4)  //DLC: 0
#define J60_MOTOR_ENABLE (0x04 << 4)    //DLC: 0
#define J60_MOTOR_CONTROL (0x08 << 4)   //DLC: 8, null data
#define J60_GET_CONFIG (0x30 << 4)  //DLC: 0
#define J60_SET_CAN_TIMEOUT (0x12 << 4) //DLC: 1
#define J60_SET_BANDWIDTH (0x14 << 4)   //DLC: 2
#define J60_SAVE_CONFIG (0x20 << 4) //DLC: 0
#define J60_GET_STATUSWORD (0x2E << 4)  //DLC: 0
#define J60_ERROR_RESET (0x22 << 4) //DLC: 0
#define J60_SET_HOME (0x0C << 4)    //DLC: 0

#endif