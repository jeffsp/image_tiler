/// @file tiler.cc
/// @brief image tiler
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2014-07-20

#include "image_tiler.h"
#include <getopt.h>
#include <iomanip>

using namespace std;
using namespace image_tiler;

const string usage = "usage: image_tiler ...";

void write_svg (ostream &s, const size_t w, const size_t h, const polygons &wp, const vector<rgb8_pixel_t> &m)
{
    // write svg header
    s << "<!DOCTYPE html>" << endl;
    s << "<html>" << endl;
    s << "<style>" << endl;
    s << "svg" << endl;
    s << "{ padding: 0px; margin:0px;}" << endl;
    s << "</style>" << endl;
    s << "<body>" << endl;
    s << "<svg currentScale=\"1.0\" width=\"" << w << "\" height=\"" << h << "\" viewBox=\"0 0 " << w << " " << h << "\">" << endl;
    for (size_t i = 0; i < wp.size (); ++i)
    {
        // write svg polygon
        s << "<polygon points=\"";
        for (const auto &j : wp[i])
            //s << " " << ::round (j.x) << ',' << ::round (j.y);
            s << " " << j.x << ',' << j.y;
        stringstream color;
        color << "#"
            << hex
            << setfill ('0') << setw (2) << static_cast<int> (m[i][0])
            << setfill ('0') << setw (2) << static_cast<int> (m[i][1])
            << setfill ('0') << setw (2) << static_cast<int> (m[i][2]);
        s << "\" style=\"stroke:"
            << color.str ()
            << ";stroke-width:1px;fill:"
            << color.str ()
            << ";\" />"
            << endl;
    }
    s << "Sorry, your browser does not support inline SVG." << endl;
    s << "</svg>" << endl;
    s << "</body>" << endl;
    s << "</html>" << endl;
}

polygons get_window_polys (const rgb8_image_t &img, const convex_uniform_tile &t, double scale, double angle)
{
    // get locations
    const double tw = scale * t.get_width ();
    const double th = scale * t.get_height ();
    const auto locs = get_tile_locations (img.rows (), img.cols (), point (img.cols () / 2.0, img.rows () / 2.0), tw, th, angle, t.is_triangular ());
    clog << locs.size () << " tiles locations" << endl;
    // get the polygons
    const polygons all_polys = get_tiled_polygons (locs, t.get_polygons (), scale, angle);
    clog << all_polys.size () << " unclipped polygons" << endl;
    // filter out tiles that don't overlap
    const polygons window_polys = get_overlapping_polygons (img.cols (), img.rows (), all_polys);
    return window_polys;
}

struct tile
{
    rgb8_pixel_t m;
    scanlines s;
    polygon p;
};

typedef vector<tile> tiles;

tiles get_tiles (const rgb8_image_t &img, const convex_uniform_tile &t, double scale, double angle)
{
    const polygons window_polys = get_window_polys (img, t, scale, angle);
    clog << window_polys.size () << " clipped polygons" << endl;
    // clip scanlines that don't overlap
    const polygon_scanlines ps = clip_scanlines (img.cols (), img.rows (), get_polygon_scanlines (window_polys));
    clog << ps.size () << " groups of scanlines" << endl;
    // get mean pixel values
    vector<rgb8_pixel_t> m (ps.size ());
    for (size_t i = 0; i < m.size (); ++i)
        for (auto j : { 0, 1, 2 })
            m[i][j] = get_mean (img, ps[i], j);
    tiles tiles (m.size ());
    for (size_t i = 0; i < m.size (); ++i)
    {
        tiles[i].m = m[i];
        tiles[i].s = ps[i];
        tiles[i].p = window_polys[i];
    }
    return tiles;
}

void write_jpg (const string &fn, const size_t w, const size_t h, const tiles &tiles)
{
    rgb8_image_t img (h, w);
    for (size_t i = 0; i < tiles.size (); ++i)
        for (auto j : { 0, 1, 2})
            fill (img, tiles[i].s, tiles[i].m[j], j);
    write_image (fn, img);
}


int main (int argc, char **argv)
{
    try
    {
        bool list = false;
        unsigned tile_index = 10;
        double scale = 16.0;
        double angle = 10.0;
        string input_fn;
        string output_fn;

        while (1)
        {
            int option_index = 0;
            static struct option long_options[] = {
                {"help", no_argument, 0,  'h' },
                {"list", no_argument, 0,  'l' },
                {"tile-index", required_argument, 0,  't' },
                {"scale", required_argument, 0,  's' },
                {"angle", required_argument, 0,  'a' },
                {0,      0,           0,  0 }
            };

            int c = getopt_long(argc, argv, "hlt:s:a:", long_options, &option_index);
            if (c == -1)
                break;

            switch (c) {
                case 0:
                case 'h':
                    printf("option %s", long_options[option_index].name);
                    if (optarg)
                        printf(" with arg %s", optarg);
                    printf("\n");

                    return 0;
                case 'l': list = true; break;
                case 't': tile_index = atoi (optarg); break;
                case 's': scale = atof (optarg); break;
                case 'a': angle = atof (optarg); break;
            }
        }

        const tile_list tl = create_tile_list ();
        if (list)
        {
            size_t i = 0;
            for (auto t : tl)
                cout << "[" << i++ << "]\t" << t.get_name () << endl;
            return 0;
        }

        clog << argc << endl;
        clog << optind << endl;
        if (optind < argc)
            input_fn = argv[optind];
        else
            throw runtime_error ("no input filename specified");

        ++optind;

        if (optind < argc)
            output_fn = argv[optind];
        else
            throw runtime_error ("no output filename specified");

        clog << "tile " << tl[tile_index].get_name () << endl;
        clog << "scale " << scale << endl;
        clog << "angle " << angle << endl;
        clog << "reading " << input_fn << endl;

        if (tile_index >= tl.size ())
            throw runtime_error ("the tile index is invalid");

        rgb8_image_t img = read_image (input_fn);
        clog << "width " << img.cols () << endl;
        clog << "height " << img.rows () << endl;

        clog << "writing to " << output_fn << endl;
        tiles tiles = get_tiles (img, tl[tile_index], scale, angle);
        write_jpg (output_fn, img.cols (), img.rows (), tiles);
        //write_svg (s, img.cols (), img.rows (), window_polys, m);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
