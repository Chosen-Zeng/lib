#ifndef __FILTERING_H
#define __FILTERING_H

#include <float.h>

#define MEANFILTERING_NUM 200

#define MEANFILTERING_YES 1
#define MEANFILTERING_NO 0

typedef struct
{
    unsigned pos;
    float data[MEANFILTERING_NUM];
    unsigned length;
    float sum;
    unsigned max_pos, min_pos;
} MeanFiltering_t;

#define MeanFiltering_t_InitStruct {0, {FLT_MIN, FLT_MAX}, 0, 0, 0, 1}

float MeanFiltering(MeanFiltering_t *struct_MeanFiltering, float new);
int MeanFiltering_GetStatus(MeanFiltering_t *struct_MeanFiltering, float new, float err_limit);

#endif