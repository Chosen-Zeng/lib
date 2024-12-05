#include "algorithm.h"

float MeanFiltering(meanfiltering_t *struct_MeanFiltering, float new_item)
{
    if (!struct_MeanFiltering->length)
    {
        struct_MeanFiltering->pos = 0;
        struct_MeanFiltering->data[0] = FLT_MIN;
        struct_MeanFiltering->data[1] = FLT_MAX;
        struct_MeanFiltering->sum = 0;
        struct_MeanFiltering->max_pos = 0;
        struct_MeanFiltering->min_pos = 1;
    }

    if (new_item > struct_MeanFiltering->data[struct_MeanFiltering->max_pos])
        struct_MeanFiltering->max_pos = struct_MeanFiltering->pos;
    if (new_item < struct_MeanFiltering->data[struct_MeanFiltering->min_pos])
        struct_MeanFiltering->min_pos = struct_MeanFiltering->pos;

    if (struct_MeanFiltering->length == MEANFILTERING_NUM)
    {
        struct_MeanFiltering->sum += new_item - struct_MeanFiltering->data[struct_MeanFiltering->pos];
        struct_MeanFiltering->data[struct_MeanFiltering->pos] = new_item;

        if (++struct_MeanFiltering->pos == MEANFILTERING_NUM)
            struct_MeanFiltering->pos = 0;

        return struct_MeanFiltering->sum / MEANFILTERING_NUM;
    }
    else
    {
        struct_MeanFiltering->sum += struct_MeanFiltering->data[struct_MeanFiltering->length] = new_item;
        struct_MeanFiltering->length++, struct_MeanFiltering->pos++;

        if (struct_MeanFiltering->length == MEANFILTERING_NUM)
            struct_MeanFiltering->pos = 0;

        return struct_MeanFiltering->sum / struct_MeanFiltering->length;
    }
}

unsigned char MeanFiltering_GetStatus(meanfiltering_t *struct_MeanFiltering, float new_item, float err_limit)
{
    float avg = MeanFiltering(struct_MeanFiltering, new_item);
    if (struct_MeanFiltering->length == MEANFILTERING_NUM &&
        struct_MeanFiltering->data[struct_MeanFiltering->max_pos] - avg <= err_limit &&
        avg - struct_MeanFiltering->data[struct_MeanFiltering->min_pos] <= err_limit)
        return MEANFILTERING_YES;

    return MEANFILTERING_NO;
}

unsigned char MeanFiltering_GetTargetStatus(meanfiltering_t *struct_MeanFiltering, float new_item, float target, float err_limit)
{
    return ABS(MeanFiltering(struct_MeanFiltering, new_item) - target) <= err_limit;
}