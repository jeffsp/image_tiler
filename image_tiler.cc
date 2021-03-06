/// @file tiler.cc
/// @brief image tiler
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2014-07-20

#include "image_tiler.h"
#include <getopt.h>

using namespace std;
using namespace image_tiler;

const string usage = "image_tiler [options] <infile> <outfile>";

polygons get_window_polys (const rgb8_image_t &img, const convex_uniform_tile &t, double scale, double angle)
{
    // get locations
    const double tw = scale * t.get_width ();
    const double th = scale * t.get_height ();
    const auto locs = get_tile_locations (img.rows (), img.cols (), point (img.cols () / 2.0, img.rows () / 2.0), tw, th, angle, t.is_triangular ());
    std::clog << locs.size () << " tiles locations" << std::endl;
    // get the polygons
    const polygons all_polys = get_tiled_polygons (locs, t.get_polygons (), scale, angle);
    std::clog << all_polys.size () << " unclipped polygons" << std::endl;
    // filter out tiles that don't intersect
    const polygons window_polys = get_intersecting_polygons (img.cols (), img.rows (), all_polys);
    return window_polys;
}

struct image_element
{
    polygon p;
    scanlines s;
    rgb8_pixel_t m;
};

typedef std::vector<image_element> image_elements;

image_elements get_image_elements (const rgb8_image_t &img, const convex_uniform_tile &t, double scale, double angle)
{
    const polygons window_polys = get_window_polys (img, t, scale, angle);
    std::clog << window_polys.size () << " clipped polygons" << std::endl;
    // clip scanlines that don't overlap
    const polygon_scanlines ps = clip_scanlines (img.cols (), img.rows (), get_polygon_scanlines (window_polys));
    std::clog << ps.size () << " groups of scanlines" << std::endl;
    // get mean pixel values
    std::vector<rgb8_pixel_t> m (ps.size ());
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

void write_jpg (const std::string &fn, const size_t w, const size_t h, const image_elements &e)
{
    rgb8_image_t img (h, w);
    for (size_t i = 0; i < e.size (); ++i)
        for (auto j : { 0, 1, 2})
            fill (img, e[i].s, e[i].m[j], j);
    write_image (fn, img);
}

void write_svg (std::ostream &s, const size_t w, const size_t h, const image_elements &e)
{
    // write svg header
    s << "<svg currentScale=\"1.0\" width=\"" << w << "\" height=\"" << h << "\" viewBox=\"0 0 " << w << " " << h << "\">" << std::endl;
    for (size_t i = 0; i < e.size (); ++i)
    {
        // write svg polygon
        s << "<polygon points=\"";
        for (const auto &j : e[i].p)
            s << " " << j.x << ',' << j.y;
        std::stringstream color;
        color << "#"
            << std::hex
            << std::setfill ('0') << std::setw (2) << static_cast<int> (e[i].m[0])
            << std::setfill ('0') << std::setw (2) << static_cast<int> (e[i].m[1])
            << std::setfill ('0') << std::setw (2) << static_cast<int> (e[i].m[2]);
        s << "\" style=\"stroke:"
            << color.str ()
            << ";stroke-width:1px;fill:"
            << color.str ()
            << ";\" />"
            << std::endl;
    }
    s << "Sorry, your browser does not support inline SVG." << std::endl;
    s << "</svg>" << std::endl;
}

void write_svg (const std::string &fn, const size_t w, const size_t h, const image_elements &e)
{
    std::ofstream ofs (fn.c_str ());
    if (!ofs)
        throw std::runtime_error ("could not open file for writing");
    write_svg (ofs, w, h, e);
}

int main (int argc, char **argv)
{
    try
    {
        // output file type
        enum class of { svg, jpeg } output_format = of::jpeg;
        // show list of tiles
        bool list = false;
        // other options
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
                {"jpeg", no_argument, 0,  'j' },
                {"svg",  no_argument, 0,  'v' },
                {"list", no_argument, 0,  'l' },
                {"tile-index", required_argument, 0,  't' },
                {"scale", required_argument, 0,  's' },
                {"angle", required_argument, 0,  'a' },
                {0,      0,           0,  0 }
            };

            int c = getopt_long(argc, argv, "hjvlt:s:a:", long_options, &option_index);
            if (c == -1)
                break;

            switch (c) {
                default:
                case 0:
                case 'h':
                {
                    clog << "usage:" << endl << '\t' << usage << endl << endl;
                    clog << "options:" << endl;
                    for (size_t i = 0; i + 1 < sizeof (long_options) / sizeof (struct option); ++i)
                    {
                        clog << "\t--" << long_options[i].name << "|-" << char (long_options[i].val);
                        if (long_options[i].has_arg)
                            clog << " <arg>" << endl;
                        else
                            clog << endl;
                    }
                    if (c != 'h')
                        throw runtime_error ("invalid option");
                    return 0;
                }
                case 'l': list = true; break;
                case 'j': output_format = of::jpeg; break;
                case 'v': output_format = of::svg; break;
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

        switch (output_format)
        {
            default: throw runtime_error ("Unknown output type");
            case of::jpeg:
            clog << "output_format: " << "jpeg" << endl;
            break;
            case of::svg:
            clog << "output_format: " << "svg" << endl;
            break;
        }

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
        clog << "writing to " << output_fn << endl;
        switch (output_format)
        {
            default: throw runtime_error ("Unknown output type");
            case of::jpeg: write_jpg (output_fn, img.cols (), img.rows (), e); break;
            case of::svg: write_svg (output_fn, img.cols (), img.rows (), e); break;
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
    }
    return -1;
}
