/// @file test_graphics.cc
/// @brief test graphics functionality
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2014-03-03

#include "graphics.h"
#include "verify.h"
#include <iostream>
#include <stdexcept>

using namespace image_tiler;
using namespace std;

void test1 ()
{
    line l1 (point (0, 0), point (0, 10));
    line l2 (point (10, 0), point (10, 5));
    line l3 (point (10, 5), point (10, 10));
    scanlines s = get_intersecting_scanlines (l1, l2);
    for (int i = 0; i < int (s.size ()); ++i)
        VERIFY (s[i].y == i && s[i].x == 0 && s[i].len == 10);
    s = get_intersecting_scanlines (l1, l3);
    for (int i = 0; i < int (s.size ()); ++i)
        VERIFY (s[i].y == i + 5 && s[i].x == 0 && s[i].len == 10);
}

void test2 (bool verbose)
{
    polygon p;
    p.push_back (point (0.1, 0.2));
    p.push_back (point (20, 0.3));
    p.push_back (point (20.3, 10.9));
    p.push_back (point (5.7, 20.3));
    p.push_back (point (-3.2, 7.6));
    scanlines s = get_convex_polygon_scanlines (p);
    if (verbose)
        for (auto i : s)
            clog << i << endl;
    VERIFY (s.size () == 20);
}

void test3 (bool verbose)
{
    polygon p;
    p.push_back (point (0.1, 0.2));
    p.push_back (point (20, 0.3));
    p.push_back (point (20.3, 10.9));
    p.push_back (point (5.7, 20.3));
    p.push_back (point (-3.2, 7.6));
    scanlines s = get_convex_polygon_scanlines (p);
    if (verbose)
        for (auto i : s)
            clog << i << endl;
    VERIFY (s.size () == 20);
}

void test4 ()
{
    const int x = 2;
    const int y = 3;
    const int w = 4;
    const int h = 5;
    rect a (x, y, w, h);
    // four corners
    VERIFY (intersects (a, rect (x, y, 1, 1)));
    VERIFY (intersects (a, rect (x+w-1, y, 1, 1)));
    VERIFY (intersects (a, rect (x, y+h-1, 1, 1)));
    VERIFY (intersects (a, rect (x+w-1, y+h-1, 1, 1)));
    // just outside of four corners
    VERIFY (!intersects (a, rect (x-1, y, 1, 1)));
    VERIFY (!intersects (a, rect (x, y-1, 1, 1)));
    VERIFY (!intersects (a, rect (x-1, y-1, 1, 1)));
    VERIFY (!intersects (a, rect (x+w, y, 1, 1)));
    VERIFY (!intersects (a, rect (x+w-1, y-1, 1, 1)));
    VERIFY (!intersects (a, rect (x, y+h, 1, 1)));
    VERIFY (!intersects (a, rect (x-1, y+h-1, 1, 1)));
    VERIFY (!intersects (a, rect (x+w, y+h, 1, 1)));
    VERIFY (!intersects (a, rect (x+w-1, y+h, 1, 1)));
    VERIFY (!intersects (a, rect (x+w, y+h-1, 1, 1)));
}

int main (int argc, char **)
{
    const bool verbose = (argc != 1);
    try
    {
        test1 ();
        test2 (verbose);
        test3 (verbose);
        test4 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
