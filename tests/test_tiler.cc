/// @file test_tilings.cc
/// @brief test tilings functionality
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2014-03-03

#include "graphics.h"
#include "image.h"
#include "opencv_utils.h"
#include "tiler.h"
#include "tiles.h"
#include "verify.h"
#include <iostream>
#include <stdexcept>

using namespace image_tiler;
using namespace std;

void test0 ()
{
    auto l = get_tile_locations (8, 3, point (), 1.0, 1.0, 0.0, false);
    // pad by one tile on each side
    // (1 + 8 + 1) * (1 + 3 + 1)
    VERIFY (l.size () == 50);
    l = get_tile_locations (8, 3, point (), 1.0, 1.0, 0.0, true);
    // now every other row has an extra tile
    // (1 + 8 + 1) * (1 + 3 + 1) + (1 + 8 + 1) / 2
    VERIFY (l.size () == 55);
    l = get_tile_locations (8, 4, point (), 2.0, 2.0, 0.0, false);
    // (1 + 4 + 1) * (1 + 2 + 1)
    VERIFY (l.size () == 24);
    l = get_tile_locations (8, 4, point (), 1.0, 2.0, 0.0, false);
    // (1 + 4 + 1) * (1 + 4 + 1)
    VERIFY (l.size () == 36);
    l = get_tile_locations (8, 4, point (), 2.0, 1.0, 0.0, false);
    // (1 + 8 + 1) * (1 + 2 + 1)
    VERIFY (l.size () == 40);
}

void test1 ()
{
    unsigned w = 200;
    unsigned h = 200;
    const polygon screen { point (0, 0), point (w, 0), point (w, h), point (0, h) };
    unsigned border = 100;
    rgb8_image_t img (h + border * 2, w + border * 2);
    img.assign (255);

    double tw = 40;
    double th = 40;
    double angle = 10;
    auto l = get_tile_locations (h, w, point (w / 2.0, h / 2.0), tw, th, angle, false);

    const polygons polys { polygon { point (0, 0), point (tw, 0), point (tw, th), point (0, th) } };

    auto g = get_tiled_polygons (l, polys, angle);

    // draw polygons
    for (auto i : g)
        draw_lines (img, i + point (border, border), rgb8_pixel_t ({212, 212, 212}));

    // draw screen outline
    draw_lines (img, screen + point (border, border), rgb8_pixel_t ({200, 200, 255}));

    // draw red dots at tile locations
    for (auto i : l)
    {
        point pt = i + point (border, border);
        size_t px = round (pt.x);
        size_t py = round (pt.y);
        if (px < img.cols () && py < img.rows ())
        {
            img (py, px, 0) = 255;
            img (py, px, 1) = 0;
            img (py, px, 2) = 0;
        }
    }

    const char *name = "test";
    cv::imshow (name, image_to_mat (img));
    cv::waitKey (0);
    cv::destroyWindow (name);
}

void test2 ()
{
    unsigned w = 200;
    unsigned h = 200;
    const polygon screen { point (0, 0), point (w, 0), point (w, h), point (0, h) };
    unsigned border = 100;
    rgb8_image_t img (h + border * 2, w + border * 2);
    img.assign (255);

    floret_pentagonal p;
    double scale = 30;
    double tw = scale * p.get_width ();
    double th = scale * p.get_height ();
    double angle = 10;
    auto l = get_tile_locations (h, w, point (w / 2.0, h / 2.0), tw, th, angle, true);

    polygons polys = p.get_polygons ();
    for (auto &i : polys)
        i = ::scale (i, scale);
    auto g = get_tiled_polygons (l, polys, angle);

    // draw polygons
    for (auto i : g)
        draw_lines (img, i + point (border, border), rgb8_pixel_t ({212, 212, 212}));

    // draw screen outline
    draw_lines (img, screen + point (border, border), rgb8_pixel_t ({200, 200, 255}));

    // draw red dots at tile locations
    for (auto i : l)
    {
        point pt = i + point (border, border);
        size_t px = round (pt.x);
        size_t py = round (pt.y);
        if (px < img.cols () && py < img.rows ())
        {
            img (py, px, 0) = 255;
            img (py, px, 1) = 0;
            img (py, px, 2) = 0;
        }
    }

    const char *name = "test";
    cv::imshow (name, image_to_mat (img));
    cv::waitKey (0);
    cv::destroyWindow (name);
}

int main ()
{
    try
    {
        test0 ();
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
