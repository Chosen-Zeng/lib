#ifndef __FLTR_H
#define __FLTR_H

#define MOVAVGFLTR_NUM_DEFAULT 32

typedef struct {
    unsigned short size, len, pos, max_pos, min_pos;
    float sum, *data;
} MovAvgFltr_t;

float *MovAvgFltr_Init(MovAvgFltr_t *const MovAvgFltr_struct);
float MovAvgFltr(MovAvgFltr_t *const MovAvgFltr_struct, const float NewData);
unsigned char MovAvgFltr_GetStatus(MovAvgFltr_t *const MovAvgFltr_struct, const float err_lim);
unsigned char MovAvgFltr_GetNewStatus(MovAvgFltr_t *const MovAvgFltr_struct, const float NewData, const float err_lim);
unsigned char MovAvgFltr_GetTargetStatus(MovAvgFltr_t *const MovAvgFltr_struct, const float target, const float err_lim);
unsigned char MovAvgFltr_GetNewTargetStatus(MovAvgFltr_t *const MovAvgFltr_struct, const float NewData, const float target, const float err_lim);
void MovAvgFltr_Clear(MovAvgFltr_t *const MovAvgFltr_struct);
float MovAvgFltr_GetData(MovAvgFltr_t *const MovAvgFltr_struct);

#endif