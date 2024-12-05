#ifndef __ALGORITHM_H
#define __ALGORITHM_H

#include "filtering.h"

/** @defgroup macro replacement
 * @{
 */

// limit X to +-Y
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
#define LIMIT_ABS(X, Y) \
    if (ABS(X) > Y)     \
        X >= 0 ? (X = Y) : (X = -Y);

/** }@ */
#endif