#ifndef __TIM_H
#define __TIM_H

#ifdef TIMsw // define TIMER TIMx, TIM frequency must be set 1Hz
typedef struct
{
    float intvl, curr, prev;
} TIMsw_t;

#define TIMsw_InitStruct {0, 0, 0}

#define TIMsw_TIME ((float)(TIMsw->CNT + 1) / (TIMsw->ARR + 1))

//  @brief     update intvl of the specific time struct
//  @attention intvl must < 1s
static inline float TIMsw_GetIntvl(TIMsw_t *const timer_struct)
{
    if (timer_struct->prev != 0 && timer_struct->curr != 0)
    {
        timer_struct->prev = timer_struct->curr;
        timer_struct->curr = TIMsw_TIME;

        if (timer_struct->prev > timer_struct->curr)
            timer_struct->intvl = 1 + timer_struct->curr - timer_struct->prev;
        else
            timer_struct->intvl = timer_struct->curr - timer_struct->prev;
    }
    else
        timer_struct->prev = timer_struct->curr = TIMsw_TIME;

    return timer_struct->intvl;
}

//  @brief     update elapse of the specific time struct
//  @attention intvl must < 1s
static inline float TIMsw_GetElapse(TIMsw_t *const timer_struct)
{
    if (timer_struct->prev != 0 && timer_struct->curr != 0)
    {
        timer_struct->prev = timer_struct->curr;
        timer_struct->curr = TIMsw_TIME;

        if (timer_struct->prev > timer_struct->curr)
            timer_struct->intvl += 1 + timer_struct->curr - timer_struct->prev;
        else
            timer_struct->intvl += timer_struct->curr - timer_struct->prev;
    }
    else
        timer_struct->prev = timer_struct->curr = TIMsw_TIME;

    return timer_struct->intvl;
}

// @return timeout or not
static inline unsigned char TIMsw_CheckTimeout(TIMsw_t *const timer_struct, const float timeout)
{
    TIMsw_GetElapse(timer_struct);
    return timer_struct->intvl >= timeout;
}

static inline void TIMsw_Clear(TIMsw_t *const timer_struct)
{
    timer_struct->intvl = timer_struct->curr = timer_struct->prev = 0;
}

static inline float TIMsw_GetRatio(TIMsw_t *const timer_struct, const float duration)
{
    return TIMsw_GetElapse(timer_struct) > duration ? 1 : timer_struct->intvl / duration;
}

#else
#error No TIM for timer defined.
#endif
#endif