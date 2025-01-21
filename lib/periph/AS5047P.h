#ifndef __AS5047P_H
#define __AS5047P_H

#include "user.h"
#include "SPI.h"

/** @defgroup AS5047P_reg
 *  @{
 */

/** @brief Volatile Registers */
#define AS5047P_NOP 0x0000
#define AS5047P_ERRFL 0x0001
#define AS5047P_PROG 0x0003
#define AS5047P_DIAAGC 0x3FFC
#define AS5047P_MAG 0x3FFD
#define AS5047P_ANGLEUNC 0x3FFE
#define AS5047P_ANGLECOM 0x3FFF

/** @brief Non-Volatile Registers(OTP) */
#define AS5047P_ZPOSM 0x0016
#define AS5047P_ZPOSL 0x0017
#define AS5047P_SETTINGS1 0x0018
#define AS5047P_SETTINGS2 0x0019

/** @brief SPI command complement */
#define AS5047P_W 0x0000
#define AS5047P_R 0x4000
/** @} */

typedef union
{
    struct
    {
        uint8_t FRERR : 1;
        uint8_t INVCOMM : 1;
        uint8_t PARERR : 1;
        uint8_t : 5;
    } ERRFL;
    struct
    {
        uint8_t PROGEN : 1;
        uint8_t : 1;
        uint8_t OTPREF : 1;
        uint8_t PROGOTP : 1;
        uint8_t : 2;
        uint8_t PROGVER : 1;
        uint8_t : 1;
    } PROG;
    struct
    {
        uint16_t AGC : 8;
        uint16_t LF : 1;
        uint16_t COF : 1;
        uint16_t MAGH : 1;
        uint16_t MAGL : 1;
        uint16_t : 4;
    } DIAAGC;
    struct
    {
        uint16_t MAG : 14;
        uint16_t : 2;

    } MAG;
    struct
    {
        uint16_t ANGLEUNC : 14;
        uint16_t : 2;
    } ANGLEUNC;
    struct
    {
        uint16_t ANGLECOM : 14;
        uint16_t : 2;
    } ANGLECOM;
} AS5047P_t_VLT;

typedef union
{
    struct
    {
        uint8_t ZPOSM : 8;
    } ZPOSM;
    struct
    {
        uint8_t ZPOSL : 6;
        uint8_t comp_l_error_en : 1;
        uint8_t comp_h_error_en : 1;
    } ZPOSL;
    struct
    {
        uint8_t : 2;
        uint8_t DIR : 1;
        uint8_t UVW_ABI : 1;
        uint8_t DAECDIS : 1;
        uint8_t ABIBIN : 1;
        uint8_t Dataselect : 1;
        uint8_t PWMon : 1;
    } SETTINGS1;
    struct
    {
        uint8_t UVWPP : 3;
        uint8_t HYS : 2;
        uint8_t ABIRES : 3;
    } SETTINGS2;
} AS5047P_t_nVLT;

typedef union
{
    AS5047P_t_VLT AS5047P_VLT;
    AS5047P_t_nVLT AS5047P_nVLT;
} AS5047P_t;

unsigned char AS5047P_ReadData(SPI_TypeDef *SPI_handle, unsigned short AS5047P_reg);

#endif