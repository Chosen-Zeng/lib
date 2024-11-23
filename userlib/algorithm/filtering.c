#include "filtering.h"

float MeanFiltering(MeanFiltering_t *struct_MeanFiltering, float new)
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

    if (new > struct_MeanFiltering->data[struct_MeanFiltering->max_pos])
        struct_MeanFiltering->max_pos = struct_MeanFiltering->pos;
    if (new < struct_MeanFiltering->data[struct_MeanFiltering->min_pos])
        struct_MeanFiltering->min_pos = struct_MeanFiltering->pos;

    if (struct_MeanFiltering->length == MEANFILTERING_NUM)
    {
        struct_MeanFiltering->sum += new - struct_MeanFiltering->data[struct_MeanFiltering->pos];
        struct_MeanFiltering->data[struct_MeanFiltering->pos] = new;

        if (++struct_MeanFiltering->pos == MEANFILTERING_NUM)
            struct_MeanFiltering->pos = 0;

        return struct_MeanFiltering->sum / MEANFILTERING_NUM;
    }
    else
    {
        struct_MeanFiltering->sum += struct_MeanFiltering->data[struct_MeanFiltering->length] = new;
        struct_MeanFiltering->length++, struct_MeanFiltering->pos++;

        if (struct_MeanFiltering->length == MEANFILTERING_NUM)
            struct_MeanFiltering->pos = 0;

        return struct_MeanFiltering->sum / struct_MeanFiltering->length;
    }
}

unsigned char MeanFiltering_GetStatus(MeanFiltering_t *struct_MeanFiltering, float new, float err_limit)
{
    float avg = MeanFiltering(struct_MeanFiltering, new);
    if (struct_MeanFiltering->length == MEANFILTERING_NUM &&
        struct_MeanFiltering->data[struct_MeanFiltering->max_pos] - avg <= err_limit &&
        avg - struct_MeanFiltering->data[struct_MeanFiltering->min_pos] <= err_limit)
        return MEANFILTERING_YES;

    return MEANFILTERING_NO;
}