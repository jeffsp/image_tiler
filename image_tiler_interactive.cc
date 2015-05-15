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
#include <iostream>
#include <stdexcept>

const char *usage = "pgm <image>";

using namespace image_tiler;
using namespace std;

struct image_element
{
    polygon p;
    scanlines s;
    rgb8_pixel_t m;
};

typedef std::vector<image_element> image_elements;

template<typename T>
T draw_polys (const T &img, const polygons &all_polys, const rgb8_pixel_t &p)
{
    T tmp (img);

    // draw polygons
    for (auto i : all_polys)
        draw_lines (tmp, i, p);

    return tmp;
}

int main (int argc, char **argv)
{
    try
    {
        if (argc != 2)
            throw runtime_error (usage);

        const string fn (argv[1]);

        rgb8_image_t original = read_image (fn);

        clog << "input file: " << fn << endl;

        const size_t w = original.cols ();
        const size_t h = original.rows ();

        clog << "dimensions: " << w << " X " << h << endl;

        tile_list tl = create_tile_list ();

        int ch = -1;
        const char *window_name = "Image Tiler";
        int tile_number = 10;
        bool outline = false;
        double scale = 30.0;
        double angle = 0.0;
        double xoffset = 0;
        double yoffset = 0;
        while (ch != 27)
        {
            const convex_uniform_tile p = tl[tile_number];
            const double tw = scale * p.get_width ();
            const double th = scale * p.get_height ();
            const auto locs = get_tile_locations (h, w, point (xoffset + w / 2.0, yoffset + h / 2.0), tw, th, angle, p.is_triangular ());
            const polygons all_polys = get_tiled_polygons (locs, p.get_polygons (), scale, angle);
            const polygons window_polys = get_overlapping_polygons (w, h, all_polys);
            const polygon_scanlines ps = clip_scanlines (w, h, get_polygon_scanlines (window_polys));

            // get mean pixel values
            std::vector<rgb8_pixel_t> m (ps.size ());
            for (size_t i = 0; i < m.size (); ++i)
                for (auto j : { 0, 1, 2 })
                    m[i][j] = get_mean (original, ps[i], j);
            image_elements e (m.size ());
            for (size_t i = 0; i < m.size (); ++i)
            {
                e[i].p = window_polys[i];
                e[i].s = ps[i];
                e[i].m = m[i];
            }

            rgb8_image_t img (h, w);
            for (size_t i = 0; i < e.size (); ++i)
                for (auto j : { 0, 1, 2})
                    fill (img, e[i].s, e[i].m[j], j);

            if (outline)
                img = draw_polys (img, all_polys, {212, 212, 212});

            cv::imshow (window_name, image_to_mat (img));
            ch = cv::waitKey (0);

            switch (ch)
            {
                case 32: { tile_number = (tile_number + 1) % tl.size (); } break;
                case 'l': { outline = !outline; } break;
                case 'q': { angle += 1; } break;
                case 'a': { angle -= 1; } break;
                case 'w': { scale += 10; } break;
                case 's': { scale = scale > 20 ? scale - 10 : 10; } break;
                case 'e': { xoffset += 1; } break;
                case 'd': { xoffset -= 1; } break;
                case 'r': { yoffset += 1; } break;
                case 'f': { yoffset -= 1; } break;
                case 'o':
                {
                    const string output_fn = "out.png";
                    clog << "writing to " << output_fn << endl;
                    write_image (output_fn, img);
                }
                break;
            }
        }
        cv::destroyWindow (window_name);
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
