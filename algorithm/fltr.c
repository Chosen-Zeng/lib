#include "fltr.h"
#include "algorithm.h"

#include <stdlib.h>
#warning 'calloc' used, pay attention to heap size

inline float *MovAvgFltr_Init(MovAvgFltr_t *MovAvgFltr_struct)
{
    return MovAvgFltr_struct->data = calloc(sizeof(float), MovAvgFltr_struct->size ? MovAvgFltr_struct->size : (MovAvgFltr_struct->size = MOVAVGFLTR_NUM_DEFAULT));
}

// update and get filtered data
float MovAvgFltr(MovAvgFltr_t *MovAvgFltr_struct, float NewData)
{
    if (!MovAvgFltr_struct->data &&
        !MovAvgFltr_Init(MovAvgFltr_struct))
        return 0;

    // store new data
    if (MovAvgFltr_struct->len == MovAvgFltr_struct->size)
    {
        MovAvgFltr_struct->sum += NewData - MovAvgFltr_struct->data[MovAvgFltr_struct->pos];
        MovAvgFltr_struct->data[MovAvgFltr_struct->pos] = NewData;
    }
    else
    {
        MovAvgFltr_struct->sum += MovAvgFltr_struct->data[MovAvgFltr_struct->pos] = NewData;
        ++MovAvgFltr_struct->len;
    }

    // update max & min position: sequential search
    if (MovAvgFltr_struct->data[MovAvgFltr_struct->pos] > MovAvgFltr_struct->data[MovAvgFltr_struct->max_pos])
        MovAvgFltr_struct->max_pos = MovAvgFltr_struct->pos;
    else if (MovAvgFltr_struct->pos == MovAvgFltr_struct->max_pos)
    {
        unsigned char temp_pos = MovAvgFltr_struct->pos;

        MovAvgFltr_struct->max_pos = temp_pos + 1 == MovAvgFltr_struct->size ? temp_pos = 0 : ++temp_pos; // set next data as max data

        while ((temp_pos + 1 == MovAvgFltr_struct->size ? temp_pos = 0 : ++temp_pos) != MovAvgFltr_struct->pos) // turn to next data if not finish a cycle
            if (MovAvgFltr_struct->data[temp_pos] > MovAvgFltr_struct->data[MovAvgFltr_struct->max_pos])
                MovAvgFltr_struct->max_pos = temp_pos;
    }
    if (MovAvgFltr_struct->data[MovAvgFltr_struct->pos] < MovAvgFltr_struct->data[MovAvgFltr_struct->min_pos])
        MovAvgFltr_struct->min_pos = MovAvgFltr_struct->pos;
    else if (MovAvgFltr_struct->pos == MovAvgFltr_struct->min_pos)
    {
        unsigned char temp_pos = MovAvgFltr_struct->pos;

        MovAvgFltr_struct->min_pos = temp_pos + 1 == MovAvgFltr_struct->size ? temp_pos = 0 : ++temp_pos; // set next data as max data

        while ((temp_pos + 1 == MovAvgFltr_struct->size ? temp_pos = 0 : ++temp_pos) != MovAvgFltr_struct->pos) // turn to next data if not finish a cycle
            if (MovAvgFltr_struct->data[temp_pos] < MovAvgFltr_struct->data[MovAvgFltr_struct->min_pos])
                MovAvgFltr_struct->min_pos = temp_pos;
    }

    // move to next position
    if (++MovAvgFltr_struct->pos == MovAvgFltr_struct->size)
    {
        // new cycle
        MovAvgFltr_struct->sum = MovAvgFltr_struct->pos = 0;

        // calculate sum
        for (unsigned char temp_pos = 0; temp_pos < MovAvgFltr_struct->len; ++temp_pos)
            MovAvgFltr_struct->sum += MovAvgFltr_struct->data[temp_pos];
    }

    return MovAvgFltr_struct->sum / MovAvgFltr_struct->len;
}

// @return filtered data within limit or not
unsigned char MovAvgFltr_GetStatus(MovAvgFltr_t *MovAvgFltr_struct, float err_lim)
{
    if (MovAvgFltr_struct->len != MovAvgFltr_struct->size) // not fully filtered
        return 0;

    float avg = MovAvgFltr_struct->sum / MovAvgFltr_struct->len;

    return MovAvgFltr_struct->data[MovAvgFltr_struct->max_pos] - avg <= err_lim && // max within limit
           avg - MovAvgFltr_struct->data[MovAvgFltr_struct->min_pos] <= err_lim;   // min within limit
}

// filter data, return whether within limit or not
unsigned char MovAvgFltr_GetNewStatus(MovAvgFltr_t *MovAvgFltr_struct, float NewData, float err_lim)
{
    if (!MovAvgFltr_struct->data &&
        !MovAvgFltr_Init(MovAvgFltr_struct))
        return 0;

    float avg = MovAvgFltr(MovAvgFltr_struct, NewData);

    return MovAvgFltr_struct->len == MovAvgFltr_struct->size &&                    // fully filtered
           MovAvgFltr_struct->data[MovAvgFltr_struct->max_pos] - avg <= err_lim && // max within limit
           avg - MovAvgFltr_struct->data[MovAvgFltr_struct->min_pos] <= err_lim;   // min within limit
}

// @return filtered data within limit or not
unsigned char MovAvgFltr_GetTargetStatus(MovAvgFltr_t *MovAvgFltr_struct, float target, float err_lim)
{
    if (MovAvgFltr_struct->len != MovAvgFltr_struct->size) // not fully filtered
        return 0;

    return ABS(MovAvgFltr_struct->sum / MovAvgFltr_struct->len - target) <= err_lim;
}

// filter data, return whether within limit or not
unsigned char MovAvgFltr_GetNewTargetStatus(MovAvgFltr_t *MovAvgFltr_struct, float NewData, float target, float err_lim)
{
    if (!MovAvgFltr_struct->data &&
        !MovAvgFltr_Init(MovAvgFltr_struct))
        return 0;

    return ABS(MovAvgFltr(MovAvgFltr_struct, NewData) - target) <= err_lim &&
           MovAvgFltr_struct->len == MovAvgFltr_struct->size; // fully filtered
}

// clear filter
inline void MovAvgFltr_Clear(MovAvgFltr_t *MovAvgFltr_struct)
{
    MovAvgFltr_struct->pos = MovAvgFltr_struct->sum = MovAvgFltr_struct->len = 0;
}

// get filtered data
inline float MovAvgFltr_GetData(MovAvgFltr_t *MovAvgFltr_struct)
{
    return MovAvgFltr_struct->len ? MovAvgFltr_struct->sum / MovAvgFltr_struct->len : 0;
}