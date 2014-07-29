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

void process (ostream &s, const rgb8_image_t &img, const convex_uniform_tile &t, double scale, double angle)
{
    // split into channels
    grayscale8_image_t p[3];
    for (auto i : { 0, 1, 2 })
        p[i] = get_channel (img, i);
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
    clog << window_polys.size () << " clipped polygons" << endl;
    // clip scanlines that don't overlap
    const polygon_scanlines ps = clip_scanlines (img.cols (), img.rows (), get_polygon_scanlines (window_polys));
    clog << ps.size () << " groups of scanlines" << endl;
    // get mean values for each channel
    vector<unsigned> mr (ps.size ());
    vector<unsigned> mg (ps.size ());
    vector<unsigned> mb (ps.size ());
    for (size_t i = 0; i < window_polys.size (); ++i)
    {
        mr[i] = get_mean (p[0], ps[i]);
        mg[i] = get_mean (p[1], ps[i]);
        mb[i] = get_mean (p[2], ps[i]);
    }
    //transform (ps.begin (), ps.end (), mr.begin (), [&] (const scanlines &a) { return get_mean (p[0], a); });
    //transform (ps.begin (), ps.end (), mg.begin (), [&] (const scanlines &a) { return get_mean (p[1], a); });
    //transform (ps.begin (), ps.end (), mb.begin (), [&] (const scanlines &a) { return get_mean (p[2], a); });
    // now window_polys contain the polygons, and mr, mg, and mb contain the mean values
    assert (window_polys.size () == mr.size ());
    assert (window_polys.size () == mg.size ());
    assert (window_polys.size () == mb.size ());
    // write svg header
    s << "<!DOCTYPE html>" << endl;
    s << "<html>" << endl;
    s << "<style>" << endl;
    s << "svg" << endl;
    s << "{ padding: 0px; margin:0px;}" << endl;
    s << "</style>" << endl;
    s << "<body>" << endl;
    s << "<svg currentScale=\"0.5\" width=\"" << img.cols () << "\" height=\"" << img.rows () << "\" viewBox=\"0 0 " << img.cols () << " " << img.rows () << "\">" << endl;
    for (size_t i = 0; i < window_polys.size (); ++i)
    {
        // write svg polygon
        s << "<polygon points=\"";
        for (const auto &j : window_polys[i])
            //s << " " << ::round (j.x) << ',' << ::round (j.y);
            s << " " << j.x << ',' << j.y;
        s << "\" style=\"stroke:#"
            << hex
            << setfill ('0') << setw (2) << mr[i]
            << setfill ('0') << setw (2) << mg[i]
            << setfill ('0') << setw (2) << mb[i]
            << dec
            << ";stroke-width:1px;fill:#"
            << hex
            << setfill ('0') << setw (2) << mr[i]
            << setfill ('0') << setw (2) << mg[i]
            << setfill ('0') << setw (2) << mb[i]
            << dec
            << ";\" />"
            << endl;
    }
    s << "Sorry, your browser does not support inline SVG." << endl;
    s << "</svg>" << endl;
    s << "</body>" << endl;
    s << "</html>" << endl;
}

int main (int argc, char **argv)
{
    try
    {
        bool list = false;
        unsigned tile_index = 10;
        double scale = 16.0;
        double angle = 10.0;
        string fn;

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

        if (optind + 1 == argc)
            fn = argv[optind];
        else
            throw runtime_error ("no filename specified");

        clog << "tile " << tl[tile_index].get_name () << endl;
        clog << "scale " << scale << endl;
        clog << "angle " << angle << endl;
        clog << "reading " << fn << endl;

        if (tile_index >= tl.size ())
            throw runtime_error ("the tile index is invalid");

        rgb8_image_t img = read_image (fn);
        clog << "width " << img.cols () << endl;
        clog << "height " << img.rows () << endl;

        process (cout, img, tl[tile_index], scale, angle);

        clog << "success" << endl;

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
