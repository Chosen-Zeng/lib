#ifndef __ALGORITHM_H
#define __ALGORITHM_H

#include "filtering.h"
#include "CRC.h"

/** @defgroup macro replacement
 * @{
 */

#define ABS(X) ((X) >= 0 ? (X) : -(X))

/** }@ */

/** @defgroup macro function
 * @{
 */

// limit X to 0 ~ Y
#define LIMIT(X, Y)     \
    if (Y >= 0)         \
    {                   \
        if (X < 0)      \
            X = 0;      \
        else if (X > Y) \
            X = Y;      \
    }                   \
    else                \
    {                   \
        if (X > 0)      \
            X = 0;      \
        else if (X < Y) \
            X = Y;      \
    }

// limit X to MIN ~ MAX
#define LIMIT_RANGE(X, MAX, MIN) \
    if (X > MAX)                 \
    {                            \
        X = MAX;                 \
    }                            \
    else if (X < MIN)            \
    {                            \
        X = MIN;                 \
    }

// limit X to +-Y
#define LIMIT_ABS(X, Y) \
    if (ABS(X) > (Y))   \
        X >= 0 ? (X = (Y)) : (X = -(Y));

/** }@ */
#endif