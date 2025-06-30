#ifndef __AS5047P_H
#define __AS5047P_H

#include "user.h"
#include "SPI.h"

#if defined AS5047P_NUM

#define AS5047P_NOP 0x0000
#define AS5047P_REG_ERRFL 0x0001
#define AS5047P_REG_PROG 0x0003
#define AS5047P_REG_DIAAGC 0x3FFC
#define AS5047P_REG_MAG 0x3FFD
#define AS5047P_REG_ANGLEUNC 0x3FFE
#define AS5047P_REG_ANGLECOM 0x3FFF

#define AS5047P_REG_ZPOSM 0x0016
#define AS5047P_REG_ZPOSL 0x0017
#define AS5047P_REG_SETTINGS1 0x0018
#define AS5047P_REG_SETTINGS2 0x0019

#define AS5047P_W 0x0000
#define AS5047P_R 0x4000

typedef struct
{
    float pos;
    unsigned char err : 1, MAGL : 1, MAGH : 1, AGC, PROGEN : 1;
} AS5047P_t;
extern AS5047P_t AS5047P[AS5047P_NUM];

typedef struct
{
    unsigned short msg;
    unsigned char type;
} AS5047P_msg_t;

typedef struct AS5047P_msg_node
{
    unsigned short msg;
    unsigned char type;
    struct AS5047P_msg_node *prev, *next;
} AS5047P_msg_node;

void AS5047P_FIFO_R(unsigned char AS5047P_ID, unsigned short AS5047P_reg);
void AS5047P_FIFO_W(unsigned char AS5047P_ID, unsigned short AS5047P_reg, unsigned short data);
unsigned char AS5047P_SendFIFO(SPI_TypeDef *SPI_handle, unsigned char AS5047P_ID);
void AS5047P_ProgramInit(unsigned char AS5047P_ID);
void AS5047P_ProgramDone(unsigned char AS5047P_ID);

#endif
#endif