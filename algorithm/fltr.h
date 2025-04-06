#ifndef __FLTR_H
#define __FLTR_H

#include <float.h>

#define MOVAVGFLTR_MAX 32

typedef struct
{
    unsigned len, size, pos;
    float data[MOVAVGFLTR_MAX];
    float sum;
    unsigned max_pos, min_pos;
} MovAvgFltr_t;

float MovAvgFltr(MovAvgFltr_t *MovAvgFltr_struct, float new_item);
unsigned char MovAvgFltr_GetStatus(MovAvgFltr_t *MovAvgFltr_struct, float err_lim);
unsigned char MovAvgFltr_GetNewStatus(MovAvgFltr_t *MovAvgFltr_struct, float new_item, float err_lim);
unsigned char MovAvgFltr_GetTargetStatus(MovAvgFltr_t *MovAvgFltr_struct, float target, float err_lim);
unsigned char MovAvgFltr_GetNewTargetStatus(MovAvgFltr_t *MovAvgFltr_struct, float new_item, float target, float err_lim);
void MovAvgFltr_Clear(MovAvgFltr_t *MovAvgFltr_struct);
float MovAvgFltr_GetData(MovAvgFltr_t *MovAvgFltr_struct);

#endif