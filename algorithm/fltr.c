#include "fltr.h"
#include "algorithm.h"

// update and get filtered data
float MovAvgFltr(MovAvgFltr_t *MovAvgFltr_struct, float new_data)
{
    // init filter struct
    if (!MovAvgFltr_struct->len)
    {
        MovAvgFltr_struct->pos = 0;
        MovAvgFltr_struct->data[0] = FLT_MIN;
        MovAvgFltr_struct->data[1] = FLT_MAX;
        MovAvgFltr_struct->sum = 0;
        MovAvgFltr_struct->max_pos = 0;
        MovAvgFltr_struct->min_pos = 1;
    }

    // handle exception
    if (ABS(new_data) > FLT_MAX)
        return MovAvgFltr_struct->len ? MovAvgFltr_struct->sum / MovAvgFltr_struct->len : 0;

    // update max & min position
    if (new_data > MovAvgFltr_struct->data[MovAvgFltr_struct->max_pos])
        MovAvgFltr_struct->max_pos = MovAvgFltr_struct->pos;
    if (new_data < MovAvgFltr_struct->data[MovAvgFltr_struct->min_pos])
        MovAvgFltr_struct->min_pos = MovAvgFltr_struct->pos;

    // return filtered value
    if (MovAvgFltr_struct->len == (MovAvgFltr_struct->size < 2 || MovAvgFltr_struct->size >= MOVAVGFLTR_MAX ? MOVAVGFLTR_MAX : MovAvgFltr_struct->size))
    {
        MovAvgFltr_struct->sum += new_data - MovAvgFltr_struct->data[MovAvgFltr_struct->pos];
        MovAvgFltr_struct->data[MovAvgFltr_struct->pos] = new_data;
    }
    else
    {
        MovAvgFltr_struct->sum += MovAvgFltr_struct->data[MovAvgFltr_struct->len] = new_data;
        MovAvgFltr_struct->len++;
    }

    if (++MovAvgFltr_struct->pos == (MovAvgFltr_struct->size < 2 || MovAvgFltr_struct->size >= MOVAVGFLTR_MAX ? MOVAVGFLTR_MAX : MovAvgFltr_struct->size))
        MovAvgFltr_struct->pos = 0;

    return MovAvgFltr_struct->sum / MovAvgFltr_struct->len;
}

// @return filtered data within limit or not
unsigned char MovAvgFltr_GetStatus(MovAvgFltr_t *MovAvgFltr_struct, float err_lim)
{
    // not fully filtered
    if (MovAvgFltr_struct->len != (MovAvgFltr_struct->size < 2 || MovAvgFltr_struct->size >= MOVAVGFLTR_MAX ? MOVAVGFLTR_MAX : MovAvgFltr_struct->size))
        return 0;

    float avg = MovAvgFltr_struct->sum / MovAvgFltr_struct->len;

    return MovAvgFltr_struct->data[MovAvgFltr_struct->max_pos] - avg <= err_lim && // max within limit
           avg - MovAvgFltr_struct->data[MovAvgFltr_struct->min_pos] <= err_lim;   // min within limit
}

// filter data, return whether within limit or not
unsigned char MovAvgFltr_GetNewStatus(MovAvgFltr_t *MovAvgFltr_struct, float new_data, float err_lim)
{
    float avg = MovAvgFltr(MovAvgFltr_struct, new_data);

    return MovAvgFltr_struct->len == (MovAvgFltr_struct->size < 2 || MovAvgFltr_struct->size >= MOVAVGFLTR_MAX ? MOVAVGFLTR_MAX : MovAvgFltr_struct->size) && // fully filtered
           MovAvgFltr_struct->data[MovAvgFltr_struct->max_pos] - avg <= err_lim &&                                                                            // max within limit
           avg - MovAvgFltr_struct->data[MovAvgFltr_struct->min_pos] <= err_lim;                                                                              // min within limit
}

// @return filtered data within limit or not
unsigned char MovAvgFltr_GetTargetStatus(MovAvgFltr_t *MovAvgFltr_struct, float target, float err_lim)
{
    // not fully filtered
    if (MovAvgFltr_struct->len != (MovAvgFltr_struct->size < 2 || MovAvgFltr_struct->size >= MOVAVGFLTR_MAX ? MOVAVGFLTR_MAX : MovAvgFltr_struct->size))
        return 0;

    return ABS(MovAvgFltr_struct->sum / MovAvgFltr_struct->len - target) <= err_lim;
}

// filter data, return whether within limit or not
unsigned char MovAvgFltr_GetNewTargetStatus(MovAvgFltr_t *MovAvgFltr_struct, float new_data, float target, float err_lim)
{
    return ABS(MovAvgFltr(MovAvgFltr_struct, new_data) - target) <= err_lim &&
           MovAvgFltr_struct->len == (MovAvgFltr_struct->size < 2 || MovAvgFltr_struct->size >= MOVAVGFLTR_MAX ? MOVAVGFLTR_MAX : MovAvgFltr_struct->size); // fully filtered
}

// clear filter
void MovAvgFltr_Clear(MovAvgFltr_t *MovAvgFltr_struct)
{
    MovAvgFltr_struct->len = 0;
}

// get filtered data
float MovAvgFltr_GetData(MovAvgFltr_t *MovAvgFltr_struct)
{
    return MovAvgFltr_struct->sum / MovAvgFltr_struct->len;
}