/// @file test_geometry.cc
/// @brief test geometry functionality
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2014-03-03

#include "geometry.h"
#include "verify.h"
#include <iostream>
#include <stdexcept>

using namespace image_tiler;
using namespace std;

void test1 ()
{
    const double x = -1;
    const double y = -2;
    point p (x, y);
    VERIFY (p.x == x);
    VERIFY (p.y == y);
    p *= 7;
    VERIFY (p.x == x * 7);
    VERIFY (p.y == y * 7);
    p += point (3, 2);
    VERIFY (p.x == x * 7 + 3);
    VERIFY (p.y == y * 7 + 2);
    p -= point (3, 2);
    VERIFY (p.x == x * 7);
    VERIFY (p.y == y * 7);
}

void test2 ()
{
    polygon p = {{0,0},{0,1},{1,0},{1,1}};
    p = rotate (p, 90.0);
    VERIFY (round (p[0].x) == 0.0);
    VERIFY (round (p[0].y) == 0.0);
    VERIFY (round (p[1].x) == -1.0);
    VERIFY (round (p[1].y) == 0.0);
    VERIFY (round (p[2].x) == 0.0);
    VERIFY (round (p[2].y) == 1.0);
    VERIFY (round (p[3].x) == -1.0);
    VERIFY (round (p[3].y) == 1.0);
}

int main ()
{
    try
    {
        test1 ();
        test2 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
