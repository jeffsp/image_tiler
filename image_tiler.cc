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

struct image_element
{
    polygon p;
    scanlines s;
    rgb8_pixel_t m;
};

typedef vector<image_element> image_elements;
typedef vector<const image_element *> image_element_ptrs;

image_elements get_image_elements (const rgb8_image_t &img, const convex_uniform_tile &t, double scale, double angle)
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
    image_elements e (m.size ());
    for (size_t i = 0; i < m.size (); ++i)
    {
        e[i].p = window_polys[i];
        e[i].s = ps[i];
        e[i].m = m[i];
    }
    return e;
}

void write_jpg (const string &fn, const size_t w, const size_t h, const image_element_ptrs &pe, vector<size_t> color_map)
{
    rgb8_image_t img (h, w);
    for (size_t i = 0; i < pe.size (); ++i)
        for (auto j : { 0, 1, 2})
            fill (img, pe[i]->s, pe[color_map[i]]->m[j], j);
    write_image (fn, img);
}

void write_svg (ostream &s, const size_t w, const size_t h, const image_element_ptrs &pe, vector<size_t> color_map)
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
    for (size_t i = 0; i < pe.size (); ++i)
    {
        // write svg polygon
        s << "<polygon points=\"";
        for (const auto &j : pe[i]->p)
            //s << " " << ::round (j.x) << ',' << ::round (j.y);
            s << " " << j.x << ',' << j.y;
        stringstream color;
        color << "#"
            << hex
            << setfill ('0') << setw (2) << static_cast<int> (pe[color_map[i]]->m[0])
            << setfill ('0') << setw (2) << static_cast<int> (pe[color_map[i]]->m[1])
            << setfill ('0') << setw (2) << static_cast<int> (pe[color_map[i]]->m[2]);
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

struct sort_by_distance
{
    sort_by_distance (size_t cx, size_t cy) : cx (cx), cy (cy) { }
    size_t cx;
    size_t cy;
    bool operator() (const image_element *a, const image_element *b) const
    {
        assert (a);
        assert (b);
        double dxa = cx - a->p[0].x;
        double dya = cy - a->p[0].y;
        double dxb = cx - b->p[0].x;
        double dyb = cy - b->p[0].y;
        double da = sqrt (dxa * dxa + dya * dya);
        double db = sqrt (dxb * dxb + dyb * dyb);
        return da < db;
    }
};

struct sort_by_luminance
{
    sort_by_luminance (const image_element_ptrs &pe) : pe (pe) { }
    const image_element_ptrs &pe;
    bool operator() (const size_t &a, const size_t &b) const
    {
        const double la = pe[a]->m[0] * 0.6 + pe[a]->m[1] * 0.3 + pe[a]->m[2] * 0.1;
        const double lb = pe[b]->m[0] * 0.6 + pe[b]->m[1] * 0.3 + pe[b]->m[2] * 0.1;
        return la < lb;
    }
};

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
        const image_elements e = get_image_elements (img, tl[tile_index], scale, angle);
        image_element_ptrs pe (e.size ());
        size_t n = 0;
        generate (pe.begin(), pe.end(), [&] { return &e[n++]; });
        // sort pointers to elements
        sort (pe.begin (), pe.end (), sort_by_distance (img.cols () / 2, img.rows () / 2));
        vector<size_t> color_map (pe.size ());
        n = 0;
        generate (color_map.begin(), color_map.end(), [&] { return n++; });
        sort (color_map.begin (), color_map.end (), sort_by_luminance (pe));
        clog << "writing to " << output_fn << endl;
        write_jpg (output_fn, img.cols (), img.rows (), pe, color_map);
        write_svg (cout, img.cols (), img.rows (), pe, color_map);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
