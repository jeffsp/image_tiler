/// @file test_image.cc
/// @brief test image_t functionality
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2014-03-03

#include "image.h"
#include "verify.h"
#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>
#include <list>
#include <numeric>
#include <stdexcept>
#include <vector>

using namespace image_tiler;
using namespace std;

template<typename T,size_t CHANNELS,size_t ROWS,size_t COLS>
void test1 ()
{
    image<T,CHANNELS> a;
    VERIFY (a.empty ());
    image<T,CHANNELS> b (ROWS, COLS);
    VERIFY (b.size () == ROWS*COLS*CHANNELS);
    VERIFY (b.front () == 0);
    VERIFY (b.back () == 0);
    image<T,CHANNELS> c (b);
    VERIFY (c.size () == ROWS*COLS*CHANNELS);
    VERIFY (c.size () == ROWS*COLS*CHANNELS);
    VERIFY (c.front () == 0);
    VERIFY (c.back () == 0);
    const int X = 100;
    image<T,CHANNELS> d (ROWS, COLS, X);
    VERIFY (d.front () == X);
    VERIFY (d.back () == X);
    c.assign (X);
    VERIFY (c.front () == X);
    VERIFY (c.back () == X);
    const int Y = 100;
    c.assign (Y);
    VERIFY (c.front () == Y);
    VERIFY (c.back () == Y);
    c.swap (d);
    VERIFY (c.front () == X);
    VERIFY (c.back () == X);
    VERIFY (d.front () == Y);
    VERIFY (d.back () == X);
}

template<typename T,size_t CHANNELS,size_t ROWS,size_t COLS>
void test2 ()
{
    image<T,CHANNELS> a (ROWS, COLS);
    a (0, 0, 0) = 10;
    a (0, 0, 1) = 11;
    a (0, 0, 2) = 12;
    VERIFY (a[0] == 10);
    VERIFY (a[1] == 11);
    VERIFY (a[2] == 12);
    a (0, 1, 0) = 20;
    a (0, 1, 1) = 21;
    a (0, 1, 2) = 22;
    VERIFY (a.index (0, 1, 0) == 3);
    VERIFY (a.index (0, 1, 1) == 4);
    VERIFY (a.index (0, 1, 2) == 5);
    VERIFY (a[3] == 20);
    VERIFY (a[4] == 21);
    VERIFY (a[5] == 22);
    a (ROWS-1, COLS-1, 0) = 30;
    a (ROWS-1, COLS-1, 1) = 31;
    a (ROWS-1, COLS-1, 2) = 32;
    VERIFY (*(a.end () - 3) == 30);
    VERIFY (*(a.end () - 2) == 31);
    VERIFY (*(a.end () - 1) == 32);
}

int main ()
{
    try
    {
        test1<unsigned char,1,19,23> ();
        test1<int,1,19,23> ();
        test1<double,1,19,23> ();
        test1<unsigned char,3,19,23> ();
        test1<int,3,19,23> ();
        test1<double,3,19,23> ();
        test1<unsigned char,4,19,23> ();
        test1<int,4,19,23> ();
        test1<double,4,19,23> ();

        test2<int,3,19,23> ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
