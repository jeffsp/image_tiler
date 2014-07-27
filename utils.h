/// @file utils.h
/// @brief utilities
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-12-18

#ifndef UTILS_H
#define UTILS_H

#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <sys/time.h>
#include <stdlib.h>

namespace image_tiler
{

template<typename Ty>
void cart2pol (Ty x, Ty y, Ty &r, Ty &th)
{
    th = atan2 (y, x);
    r = sqrt (x * x + y * y);
}

template<typename Ty>
void pol2cart (Ty r, Ty th, Ty &x, Ty &y)
{
    x = r * cos (th);
    y = r * sin (th);
}

template<typename T>
T clip (T x, T min, T max)
{
    return x < min ? min : (x > max) ? max : x;
}

}

#endif
