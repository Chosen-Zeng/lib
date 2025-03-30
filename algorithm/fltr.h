#ifndef __FLTR_H
#define __FLTR_H

#include <float.h>

#define MOVAVGFLTR_MAX 32

#define MOVAVGFLTR_YES 1
#define MOVAVGFLTR_NO 0

typedef struct
{
    unsigned pos;
    float data[MOVAVGFLTR_MAX];
    unsigned len, num;
    float sum;
    unsigned max_pos, min_pos;
} MovAvgFltr_t;

#define MovAvgFltr_InitStruct {0, {FLT_MIN, FLT_MAX}, 0, 0, 0, 0, 1}

float MovAvgFltr(MovAvgFltr_t *MovAvgFltr_struct, float new_item);
unsigned char MovAvgFltr_GetStatus(MovAvgFltr_t *MovAvgFltr_struct, float new_item, float err_lim);
unsigned char MovAvgFltr_GetTargetStatus(MovAvgFltr_t *MovAvgFltr_struct, float new_item, float target, float err_lim);

#endif