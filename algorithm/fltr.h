#ifndef __FLTR_H
#define __FLTR_H

#include <float.h>

#define MOVAVGFLTR_MAX 32

typedef struct
{
    unsigned char len, size, pos;
    float data[MOVAVGFLTR_MAX], sum;
    unsigned char max_pos, min_pos;
} MovAvgFltr_t;

float MovAvgFltr(MovAvgFltr_t *MovAvgFltr_struct, float NewData);
unsigned char MovAvgFltr_GetStatus(MovAvgFltr_t *MovAvgFltr_struct, float err_lim);
unsigned char MovAvgFltr_GetNewStatus(MovAvgFltr_t *MovAvgFltr_struct, float NewData, float err_lim);
unsigned char MovAvgFltr_GetTargetStatus(MovAvgFltr_t *MovAvgFltr_struct, float target, float err_lim);
unsigned char MovAvgFltr_GetNewTargetStatus(MovAvgFltr_t *MovAvgFltr_struct, float NewData, float target, float err_lim);
void MovAvgFltr_Clear(MovAvgFltr_t *MovAvgFltr_struct);
float MovAvgFltr_GetData(MovAvgFltr_t *MovAvgFltr_struct);

#endif