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

unsigned BORDER = 200;
rgb8_image_t img;
const unsigned w = 400;
const unsigned h = 300;

void init_image ()
{
    img = rgb8_image_t (h + BORDER * 2, w + BORDER * 2);
    img.assign (255);
}

void show_image ()
{
    const char *name = "test";
    cv::imshow (name, image_to_mat (img));
    cv::moveWindow (name, 100, 100);
    cv::waitKey (0);
    cv::destroyWindow (name);
}

void draw_polys (const polygons &all_polys, const rgb8_pixel_t &p)
{
    // draw polygons
    for (auto i : all_polys)
        draw_lines (img, i + point (BORDER, BORDER), p);
}

void draw_locs (const points &locs, const rgb8_pixel_t &p)
{
    // draw red dots at tile locations
    for (auto i : locs)
    {
        point pt = i + point (BORDER, BORDER);
        size_t px = round (pt.x);
        size_t py = round (pt.y);
        if (px < img.cols () && py < img.rows ())
        {
            img (py, px, 0) = p[0];
            img (py, px, 1) = p[1];
            img (py, px, 2) = p[1];
        }
    }
}

void draw_border (const rgb8_pixel_t &p)
{
    // draw screen outline
    const polygon screen { point (0, 0), point (w, 0), point (w, h), point (0, h) };
    draw_lines (img, screen + point (BORDER, BORDER), p);
}

void draw_scanlines (const polygon_scanlines &ps)
{
    for (const auto &i : ps)
    {
        const int r = rand () % 256;
        const int g = rand () % 256;
        const int b = rand () % 256;
        for (const auto &j : i)
        {
            for (size_t k = 0; k < j.len; ++k)
            {
                img (j.y + BORDER, j.x + k + BORDER, 0) = r;
                img (j.y + BORDER, j.x + k + BORDER, 1) = g;
                img (j.y + BORDER, j.x + k + BORDER, 2) = b;
            }
        }
    }
}

void test1 ()
{
    const double scale = 40.0;
    const double angle = 10.0;
    const auto locs = get_tile_locations (h, w, point (w / 2.0, h / 2.0), scale, scale, angle, false);
    const polygons tile_polys { polygon { point (0, 0), point (1.0, 0), point (1.0, 1.0), point (0, 1.0) } };
    const auto all_polys = get_tiled_polygons (locs, tile_polys, scale, angle);
    init_image ();
    draw_polys (all_polys, {212, 212, 212});
    draw_locs (locs, {255, 0, 0});
    draw_border ({200, 200, 255});
    show_image ();
}

void test2 ()
{
    const floret_pentagonal p;
    const double scale = 30.0;
    const double tw = scale * p.get_width ();
    const double th = scale * p.get_height ();
    const double angle = 10.0;
    const auto locs = get_tile_locations (h, w, point (w / 2.0, h / 2.0), tw, th, angle, p.is_triangular ());
    const polygons all_polys = get_tiled_polygons (locs, p.get_polygons (), scale, angle);
    init_image ();
    draw_polys (all_polys, {212, 212, 212});
    const polygons window_polys = get_overlapping_polygons (w, h, all_polys);
    draw_polys (window_polys, {100, 100, 100});
    draw_locs (locs, {255, 0, 0});
    draw_border ({200, 200, 255});
    show_image ();
}

void test3 ()
{
    const floret_pentagonal p;
    const double scale = 30.0;
    const double tw = scale * p.get_width ();
    const double th = scale * p.get_height ();
    const double angle = 10.0;
    const auto locs = get_tile_locations (h, w, point (w / 2.0, h / 2.0), tw, th, angle, p.is_triangular ());
    const polygons all_polys = get_tiled_polygons (locs, p.get_polygons (), scale, angle);
    const polygons window_polys = get_overlapping_polygons (w, h, all_polys);
    const polygon_scanlines ps = get_clipped_scanlines (w, h, window_polys);
    init_image ();
    draw_scanlines (ps);
    show_image ();
}

int main ()
{
    try
    {
        test0 ();
        test1 ();
        test2 ();
        test3 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
