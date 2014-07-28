/// @file tiles.h
/// @brief 2D convex uniform tilings
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-12-18

#ifndef TILES_H
#define TILES_H

#include "geometry.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

namespace image_tiler
{

enum class convex_uniform_tiling
{
    square4,
    truncated_square,
    tetrakis_square,
    snub_square,
    cairo_pentagonal,
    hexagonal,
    triangular,
    trihex,
    rhombile,
    truncated_hex,
    triakus_triangular,
    rhombitrihexagonal,
    deltoidal_trihexagonal,
    truncated_trihexagonal,
    kisrhombile,
    snub_trihexagonal,
    floret_pentagonal,
    elongated_triangular,
    max
};

class convex_uniform_tile
{
    protected:
    std::string name;
    convex_uniform_tiling id;
    polygons p;
    double width;
    double height;
    bool triangular;
    public:
    convex_uniform_tile (const std::string &name = std::string (), convex_uniform_tiling id = convex_uniform_tiling::max, size_t n = 0, bool triangular = false)
        : name (name)
        , id (id)
        , p (n)
        , triangular (triangular)
    {
    }
    bool operator== (const convex_uniform_tile &a) const
    {
        return id == a.id;
    }
    const polygons &get_polygons () const
    {
        return p;
    }
    const std::string &get_name () const { return name; }
    convex_uniform_tiling get_id () const { return id; }
    double get_width () const { return width; }
    void set_width (const double x) { width = x; }
    double get_height () const { return height; }
    void set_height (const double x) { height = x; }
    bool is_triangular () const { return triangular; }
    friend std::ostream& operator<< (std::ostream &s, const convex_uniform_tile &t)
    {
        s << "name " << t.name << std::endl;
        s << "id " << static_cast<int> (t.id) << std::endl;
        s << "polygons " << t.p << std::endl;
        s << "width " << t.width << std::endl;
        s << "height " << t.height << std::endl;
        return s;
    }
};

struct square4 : public convex_uniform_tile
{
    square4 ()
        : convex_uniform_tile ("square4", convex_uniform_tiling::square4, 4)
    {
        p[0] = square ();
        p[1] = translate (p[0], p[0][1]);
        p[2] = translate (p[0], p[0][2]);
        p[3] = translate (p[0], p[0][3]);
        set_width (2.0);
        set_height (2.0);
    }
};

struct truncated_square : public convex_uniform_tile
{
    truncated_square ()
        : convex_uniform_tile ("truncated_square", convex_uniform_tiling::truncated_square, 4)
    {
        p[0] = octagon ();
        p[1] = translate (octagon (), p[0][3]);
        p[2] = translate (square (), p[0][2]);
        p[3] = translate (square (), p[0][5]);
        set_width (2.0 * p[0][3].x);
        set_height (2.0 * p[0][3].y);
    }
};

struct tetrakis_square : public convex_uniform_tile
{
    tetrakis_square ()
        : convex_uniform_tile ("tetrakis_square", convex_uniform_tiling::tetrakis_square, 8)
    {
        p[0] = triangle90 ();
        p[1] = translate (mirrory (mirrorx (p[0])), p[0][2]);
        p[2] = translate (p[0], p[1][1]);
        p[3] = translate (p[1], p[1][1]);
        p[4] = translate (p[0], p[0][1]);
        p[5] = translate (p[1], p[0][1]);
        p[6] = translate (p[2], p[0][1]);
        p[7] = translate (p[3], p[0][1]);
        set_width (2.0);
        set_height (2.0);
    }
};

struct snub_square : public convex_uniform_tile
{
    snub_square ()
        : convex_uniform_tile ("snub_square", convex_uniform_tiling::snub_square, 12)
    {
        p[0] = triangle60 ();
        p[1] = rotate (square (), 60.0);
        p[2] = translate (rotate (square (), 30.0), p[0][1]);
        p[3] = translate (rotate (triangle60 (), 90.0), p[0][2]);
        p[4] = translate (rotate (triangle60 (), 30.0), p[0][2]);
        p[5] = translate (rotate (square (), 30.0), p[1][2]);
        p[6] = translate (rotate (square (), 60.0), p[2][2]);
        p[7] = translate (rotate (triangle60 (), 60.0), p[5][1]);
        p[8] = translate (rotate (triangle60 (), 60.0), p[1][3]);
        p[9] = translate (rotate (triangle60 (), 120.0), p[1][2]);
        p[10] = translate (rotate (triangle60 (), 150.0), p[0][0]);
        p[11] = translate (rotate (triangle60 (), -30.0), p[0][1]);
        set_width (1.0 + 2 * p[0][2].y);
        set_height (p[5][2].y);
    }
};

struct cairo_pentagonal : public convex_uniform_tile
{
    cairo_pentagonal ()
        : convex_uniform_tile ("cairo_pentagonal", convex_uniform_tiling::cairo_pentagonal, 8)
    {
        p[0] = cairo_pentagon ();
        p[1] = translate (rotate (cairo_pentagon (), 90.0), p[0][3]);
        p[2] = translate (rotate (cairo_pentagon (), -90.0), p[1][1]);
        p[3] = translate (rotate (translate (cairo_pentagon (), negate (p[0][3])), 180.0), p[1][1]);
        p[4] = translate (p[0], p[2][3]);
        p[5] = translate (p[1], p[2][3]);
        p[6] = translate (p[2], p[2][3]);
        p[7] = translate (p[3], p[2][3]);
        set_width (p[6][3].x);
        set_height (p[3][0].y);
    }
};

struct hexagonal : public convex_uniform_tile
{
    hexagonal ()
        : convex_uniform_tile ("hexagonal", convex_uniform_tiling::hexagonal, 3, true)
    {
        p[0] = hexagon ();
        p[1] = translate (p[0], p[0][4]);
        p[2] = translate (p[0], p[0][2]);
        set_width (p[2][2].x);
        set_height (get_width () * sin (deg_to_rad (60.0)));
    }
};

struct triangular : public convex_uniform_tile
{
    triangular ()
        : convex_uniform_tile ("triangular", convex_uniform_tiling::triangular, 2, true)
    {
        p[0] = triangle60 ();
        p[1] = rotate (triangle60 (), -60);
        set_width (p[0][1].x);
        set_height (get_width () * sin (deg_to_rad (60.0)));
    }
};

struct trihex : public convex_uniform_tile
{
    trihex ()
        : convex_uniform_tile ("trihex", convex_uniform_tiling::trihex, 3, true)
    {
        p[0] = triangle60 ();
        p[1] = translate (hexagon (), p[0][1]);
        p[2] = translate (rotate (triangle60 (), 60), p[0][2]);
        set_width (p[1][1].x);
        set_height (get_width () * sin (deg_to_rad (60.0)));
    }
};

struct rhombile : public convex_uniform_tile
{
    rhombile ()
        : convex_uniform_tile ("rhombile", convex_uniform_tiling::rhombile, 3, true)
    {
        p[0] = rhombus ();
        p[1] = rotate (rhombus (), 60);
        p[2] = translate (rotate (rhombus (), 120), p[0][2]);
        set_width (p[0][2].x);
        set_height (get_width () * sin (deg_to_rad (60.0)));
    }
};

struct truncated_hex : public convex_uniform_tile
{
    truncated_hex ()
        : convex_uniform_tile ("truncated_hex", convex_uniform_tiling::truncated_hex, 12, true)
    {
        p[0] = rotate (dodecagon (), -30.0);
        p[1] = translate (triangle60 (), p[0][3]);
        p[2] = translate (rotate (triangle60 (), 60), p[0][5]);
        p[3] = translate (p[0], p[1][1]);
        p[4] = translate (p[1], p[1][1]);
        p[5] = translate (p[2], p[1][1]);
        p[6] = translate (p[0], p[0][7]);
        p[7] = translate (p[1], p[0][7]);
        p[8] = translate (p[2], p[0][7]);
        p[9] = translate (p[0], p[3][7]);
        p[10] = translate (p[1], p[3][7]);
        p[11] = translate (p[2], p[3][7]);
        set_width (2.0 * p[1][1].x);
        set_height (get_width () * sin (deg_to_rad (60.0)));
    }
};

struct triakus_triangular : public convex_uniform_tile
{
    triakus_triangular ()
        : convex_uniform_tile ("triakus_triangular", convex_uniform_tiling::triakus_triangular, 6, true)
    {
        p[0] = triangle135 ();
        p[1] = translate (rotate (triangle135 (), 120.0), p[0][1]);
        p[2] = translate (rotate (triangle135 (), -120.0), p[1][1]);
        p[3] = translate (rotate (p[0], -60.0), p[1][1]);
        p[4] = translate (rotate (p[1], -60.0), p[1][1]);
        p[5] = translate (rotate (p[2], -60.0), p[1][1]);
        set_width (p[0][1].x);
        set_height (get_width () * sin (deg_to_rad (60.0)));
    }
};

struct rhombitrihexagonal : public convex_uniform_tile
{
    rhombitrihexagonal ()
        : convex_uniform_tile ("rhombitrihexagonal", convex_uniform_tiling::rhombitrihexagonal, 6, true)
    {
        p[0] = triangle60 ();
        p[1] = translate (rotate (square (), 30.0), p[0][1]);
        p[2] = translate (rotate (triangle60 (), 60.0), p[1][1]);
        p[3] = translate (rotate (square (), -30.0), p[2][0]);
        p[4] = translate (rotate (hexagon (), 30.0), p[0][2]);
        p[5] = translate (square (), p[4][1]);
        set_width (p[3][1].x);
        set_height (get_width () * sin (deg_to_rad (60.0)));
    }
};

struct deltoidal_trihexagonal : public convex_uniform_tile
{
    deltoidal_trihexagonal ()
        : convex_uniform_tile ("deltoidal_trihexagonal", convex_uniform_tiling::deltoidal_trihexagonal, 6, true)
    {
        p[0] = kite ();
        p[1] = rotate (kite (), 1 * 60.0);
        p[2] = rotate (kite (), 2 * 60.0);
        p[3] = rotate (kite (), 3 * 60.0);
        p[4] = rotate (kite (), 4 * 60.0);
        p[5] = rotate (kite (), 5 * 60.0);
        set_width (2.0);
        set_height (get_width () * sin (deg_to_rad (60.0)));
    }
};

struct truncated_trihexagonal : public convex_uniform_tile
{
    truncated_trihexagonal ()
        : convex_uniform_tile ("truncated_trihexagonal", convex_uniform_tiling::truncated_trihexagonal, 7, true)
    {
        p[0] = dodecagon ();
        p[1] = translate (rotate (hexagon (), 180.0), p[0][1]);
        p[2] = translate (rotate (square (), -60.0), p[0][1]);
        p[3] = translate (rotate (hexagon (), -60.0), p[0][2]);
        p[4] = translate (rotate (square (), 0.0), p[0][3]);
        p[5] = translate (rotate (hexagon (), 0.0), p[0][4]);
        p[6] = translate (rotate (square (), 60.0), p[0][5]);
        set_width (dodecagon ()[6].y + 1.0);
        set_height (get_width () * sin (deg_to_rad (60.0)));
    }
};

struct kisrhombile : public convex_uniform_tile
{
    kisrhombile ()
        : convex_uniform_tile ("kisrhombile", convex_uniform_tiling::kisrhombile, 12, true)
    {
        for (size_t i = 0; i < 6; ++i)
        {
            p[2 * i] = rotate (triangle30 (), i * 60);
            p[2 * i + 1] = rotate (mirrorx (triangle30 ()), i * 60);
        }
        set_width (2.0 * p[0][1].x);
        set_height (get_width () * sin (deg_to_rad (60.0)));
    }
};

struct snub_trihexagonal : public convex_uniform_tile
{
    snub_trihexagonal ()
        : convex_uniform_tile ("snub_trihexagonal", convex_uniform_tiling::snub_trihexagonal, 15, true)
    {
        p[0] = hexagon ();
        p[1] = translate (rotate (triangle60 (), -60.0), p[0][0]);
        p[2] = translate (rotate (triangle60 (), -120.0), p[0][0]);
        p[3] = translate (rotate (triangle60 (), -60.0), p[0][1]);
        p[4] = translate (rotate (triangle60 (), -120.0), p[0][1]);
        p[5] = translate (rotate (triangle60 (), -60.0), p[0][2]);
        p[6] = translate (rotate (triangle60 (), -120.0), p[0][2]);
        p[7] = translate (rotate (triangle60 (), 0.0), p[0][2]);
        p[8] = translate (rotate (triangle60 (), 60.0), p[0][2]);
        p[9] = translate (rotate (triangle60 (), 0.0), p[0][3]);
        p[10] = translate (rotate (triangle60 (), 60.0), p[0][3]);
        p[11] = translate (rotate (triangle60 (), 0.0), p[0][4]);
        p[12] = translate (rotate (triangle60 (), 60.0), p[0][4]);
        p[13] = translate (rotate (triangle60 (), -120.0), p[0][5]);
        p[14] = translate (rotate (triangle60 (), 60.0), p[0][5]);
        set_width (3.0);
        set_height (get_width () * sin (deg_to_rad (60.0)));
    }
};

struct floret_pentagonal : public convex_uniform_tile
{
    floret_pentagonal ()
        : convex_uniform_tile ("floret_pentagonal", convex_uniform_tiling::floret_pentagonal, 6, true)
    {
        const double a = 0.5 * acos (2.5 / sqrt (1.0 + 2.5 * 2.5)) * 180.0 / M_PI;
        for (int i = 0; i < 6; ++i)
            p[i] = rotate (pentagon30 (), a + i * 60.0);
        const double x = (0.5 + 2.0 * cos (deg_to_rad (60.0)));
        const double y = 2.0 * sin (deg_to_rad (60.0));
        set_width (sqrt (x * x + y * y));
        set_height (get_width () * sin (deg_to_rad (60.0)));
    }
};

struct elongated_triangular : public convex_uniform_tile
{
    elongated_triangular ()
        : convex_uniform_tile ("elongated_triangular", convex_uniform_tiling::elongated_triangular, 12)
    {
        p[0] = square ();
        p[1] = translate (square (), point (1.0, 0.0));
        p[2] = rotate (triangle60 (), -60.0);
        p[3] = translate (p[2], p[1][0]);
        p[4] = translate (triangle60 (), p[2][1]);
        p[5] = translate (triangle60 (), p[3][1]);
        p[6] = translate (rotate (square (), -90.0), p[2][1]);
        p[7] = translate (rotate (square (), -90.0), p[4][1]);
        p[8] = translate (p[2], p[6][1]);
        p[9] = translate (p[3], p[6][1]);
        p[10] = translate (p[4], p[6][1]);
        p[11] = translate (p[5], p[6][1]);
        set_width (2.0);
        set_height (2.0 * (1.0 + sin (deg_to_rad (60.0))));
    }
};

typedef std::vector<convex_uniform_tile> tile_list;

std::ostream& operator<< (std::ostream &s, const tile_list &tl)
{
    for (auto i : tl)
        s << i;
    return s;
}

tile_list create_tile_list ()
{
    tile_list t;
    t.push_back (square4 ());
    t.push_back (truncated_square ());
    t.push_back (tetrakis_square ());
    t.push_back (snub_square ());
    t.push_back (cairo_pentagonal ());
    t.push_back (hexagonal ());
    t.push_back (triangular ());
    t.push_back (trihex ());
    t.push_back (rhombile ());
    t.push_back (truncated_hex ());
    t.push_back (triakus_triangular ());
    t.push_back (rhombitrihexagonal ());
    t.push_back (deltoidal_trihexagonal ());
    t.push_back (truncated_trihexagonal ());
    t.push_back (kisrhombile ());
    t.push_back (snub_trihexagonal ());
    t.push_back (floret_pentagonal ());
    t.push_back (elongated_triangular ());
    return t;
}

}

#endif
