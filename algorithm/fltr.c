#include "fltr.h"
#include "algorithm.h"

// update and get filtered data
float MovAvgFltr(MovAvgFltr_t *MovAvgFltr_struct, float new_item)
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

    // update max & min
    if (new_item > MovAvgFltr_struct->data[MovAvgFltr_struct->max_pos])
        MovAvgFltr_struct->max_pos = MovAvgFltr_struct->pos;
    if (new_item < MovAvgFltr_struct->data[MovAvgFltr_struct->min_pos])
        MovAvgFltr_struct->min_pos = MovAvgFltr_struct->pos;

    // return filtered val
    if (MovAvgFltr_struct->len == (MovAvgFltr_struct->num < 2 ? MOVAVGFLTR_MAX : MovAvgFltr_struct->num))
    {
        MovAvgFltr_struct->sum += new_item - MovAvgFltr_struct->data[MovAvgFltr_struct->pos];
        MovAvgFltr_struct->data[MovAvgFltr_struct->pos] = new_item;

        if (++MovAvgFltr_struct->pos == (MovAvgFltr_struct->num < 2 ? MOVAVGFLTR_MAX : MovAvgFltr_struct->num))
            MovAvgFltr_struct->pos = 0;

        return MovAvgFltr_struct->sum / (MovAvgFltr_struct->num < 2 ? MOVAVGFLTR_MAX : MovAvgFltr_struct->num);
    }
    else
    {
        MovAvgFltr_struct->sum += MovAvgFltr_struct->data[MovAvgFltr_struct->len] = new_item;
        MovAvgFltr_struct->len++, MovAvgFltr_struct->pos++;

        if (MovAvgFltr_struct->len == (MovAvgFltr_struct->num < 2 ? MOVAVGFLTR_MAX : MovAvgFltr_struct->num))
            MovAvgFltr_struct->pos = 0;

        return MovAvgFltr_struct->sum / MovAvgFltr_struct->len;
    }
}

unsigned char MovAvgFltr_GetStatus(MovAvgFltr_t *MovAvgFltr_struct, float new_item, float err_lim)
{
    float avg = MovAvgFltr(MovAvgFltr_struct, new_item);
    if (MovAvgFltr_struct->len == (MovAvgFltr_struct->num < 2 ? MOVAVGFLTR_MAX : MovAvgFltr_struct->num) &&
        MovAvgFltr_struct->data[MovAvgFltr_struct->max_pos] - avg <= err_lim &&
        avg - MovAvgFltr_struct->data[MovAvgFltr_struct->min_pos] <= err_lim)
        return MOVAVGFLTR_YES;

    return MOVAVGFLTR_NO;
}

unsigned char MovAvgFltr_GetTargetStatus(MovAvgFltr_t *MovAvgFltr_struct, float new_item, float target, float err_lim)
{
    return ABS(MovAvgFltr(MovAvgFltr_struct, new_item) - target) <= err_lim;
}