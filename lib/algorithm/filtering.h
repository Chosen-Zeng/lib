#ifndef __FILTERING_H
#define __FILTERING_H

#include <float.h>

#define MEANFILTERING_NUM 100

#define MEANFILTERING_YES 1
#define MEANFILTERING_NO 0

typedef struct
{
    unsigned pos;
    float data[MEANFILTERING_NUM];
    unsigned length;
    float sum;
    unsigned max_pos, min_pos;
} meanfiltering_t;

#define MeanFiltering_InitStruct {0, {FLT_MIN, FLT_MAX}, 0, 0, 0, 1}

float MeanFiltering(meanfiltering_t *struct_MeanFiltering, float new);
unsigned char MeanFiltering_GetStatus(meanfiltering_t *struct_MeanFiltering, float new, float err_limit);
unsigned char MeanFiltering_GetTargetStatus(meanfiltering_t *struct_MeanFiltering, float new_item, float target, float err_limit);

#endif