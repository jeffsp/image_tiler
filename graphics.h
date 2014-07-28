/// @file graphics.h
/// @brief graphics
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2014-03-03

#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "image.h"
#include "geometry.h"
#include "utils.h"
#include <iostream>
#include <cstring>

namespace image_tiler
{

struct rect
{
    rect (int x, int y, unsigned width, unsigned height)
        : x (x)
        , y (y)
        , width (width)
        , height (height)
    {
    }
    int x;
    int y;
    unsigned width;
    unsigned height;
};

// io helper
std::ostream &operator<< (std::ostream &s, const rect &r)
{
    s << ' ' << r.y;
    s << ' ' << r.x;
    s << ' ' << r.width;
    s << ' ' << r.height;
    return s;
}

bool operator== (const rect &a, const rect &b)
{
    return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
}

bool intersects (const rect &a, const rect &b)
{
    const int ax1 = a.x;
    const int ax2 = a.x + a.width;
    const int ay1 = a.y;
    const int ay2 = a.y + a.height;
    const int bx1 = b.x;
    const int bx2 = b.x + b.width;
    const int by1 = b.y;
    const int by2 = b.y + b.height;
    return ax1 < bx2 && ax2 > bx1 && ay1 < by2 && ay2 > by1;
}

rect get_bounding_rect (const polygon &p)
{
    rectf r = get_bounding_rectf (p);
    return rect (::round (r.minx), ::round (r.miny), ::round (r.maxx) - ::round (r.minx), ::round (r.maxy) - ::round (r.miny));
}

bool is_close (const polygon &a, const polygon &b)
{
    const rect ra = get_bounding_rect (a);
    const rect rb = get_bounding_rect (b);
    return intersects (ra, rb);
}

bool contains (const rect &r, int x, int y)
{
    return (x >= r.x && y >= r.y && x < static_cast<int> (r.x + r.width) && y < static_cast<int> (r.y + r.height));
}

struct scanline
{
   scanline (int y = 0, int x = 0, unsigned len = 0)
       : y (y), x (x), len (len)
   {
   }
   int y;
   int x;
   unsigned len;
};

typedef std::vector<scanline> scanlines;

// io helper
std::ostream &operator<< (std::ostream &s, const scanline &l)
{
    s << ' ' << l.y;
    s << ' ' << l.x;
    s << ' ' << l.len;
    return s;
}

// solve for x
int solve_line_x (int y, const line &l)
{
    int x1 = l.p1.x;
    int x2 = l.p2.x;
    int y1 = l.p1.y;
    int y2 = l.p2.y;
    if (x1 == x2)
        return x1;
    const int PREC = 10000;
    int m = PREC * (y2 - y1) / (x2 - x1);
    if (m == 0)
        return x1;
    return x1 + PREC * (y - y1) / m;
}

// given two arbitrary lines, get the scanlines that intersect both
scanlines get_intersecting_scanlines (line l1, line l2)
{
    // make sure scanlines are in ascending order
    if (l1.p1.y > l1.p2.y)
        std::swap (l1.p1, l1.p2);
    if (l2.p1.y > l2.p2.y)
        std::swap (l2.p1, l2.p2);
    const int y1 = std::max (l1.p1.y, l2.p1.y);
    const int y2 = std::min (l1.p2.y, l2.p2.y);
    // return value
    scanlines s;
    if (y2 <= y1)
        return s;
    // this seems to help on ARMv7
    s.reserve (y2 - y1);
    // get the scanlines
    for (int y = y1; y < y2; ++y)
    {
        // solve for x intersections
        int x1 = solve_line_x (y, l1);
        int x2 = solve_line_x (y, l2);
        // make sure the scanline has non-zero length (because of rounding)
        if (x1 == x2)
            continue;
        // make sure x's are ascending
        if (x2 < x1)
            std::swap (x1, x2);
        // save the scanline
        scanline l (y, x1, x2 - x1);
        s.push_back (l);
    }
    return s;
}

scanlines get_convex_polygon_scanlines (const polygon &p)
{
    scanlines s;
    // for each line in the polygon
    for (size_t i = 0; i < p.size (); ++i)
    {
        line l1 (round (p[i]), round (p[(i + 1) % p.size ()]));
        // for each remaining line
        for (size_t j = i + 1; j < p.size (); ++j)
        {
            line l2 (round (p[j]), round (p[(j + 1) % p.size ()]));
            std::vector<scanline> t = get_intersecting_scanlines (l1, l2);
            s.insert (s.end (), t.begin (), t.end ());
        }
    }
    return s;
}

scanlines clip (const scanlines &s, const rect &r)
{
    scanlines s2;
    // reserves seem to make a difference on ARMv7
    s2.reserve (s.size ());
    for (auto i : s)
    {
        // clip scanline
        if (i.y < r.y || i.y >= static_cast<int> (r.y + r.height))
            continue;
        // clip to x boundary
        const int sx1 = i.x;
        const int rx2 = r.x + r.width;
        if (sx1 >= rx2)
            continue; // clip scanline
        const int sx2 = i.x + i.len;
        const int rx1 = r.x;
        if (rx1 >= sx2)
            continue; // clip scanline
        // it intersects the rect
        // get the intersecting segment
        const int x1 = std::max (sx1, rx1);
        const int x2 = std::min (sx2, rx2);
        assert (x1 < x2);
        s2.push_back (scanline (i.y, x1, x2 - x1));
    }
    return s2;
}

unsigned get_mean (const grayscale8_image_t &img, const scanlines &s)
{
    if (s.empty ())
        return 0;
    size_t sum = 0;
    size_t total = 0;
    for (auto i : s)
    {
        for (int x = i.x; x < static_cast<int> (i.x + i.len); ++x)
        {
            ++total;
            sum += img (i.y, x, 0);
        }
    }
    unsigned p = ::round (static_cast<double> (sum) / total);
    assert (p < 256);
    return p;
}

void fill (const scanlines &s, grayscale8_image_t &img, unsigned p)
{
    assert (!s.empty ());
    for (auto i : s)
    {
        assert (i.x >= 0);
        assert (i.y >= 0);
        assert (i.x < static_cast<int> (img.cols ()));
        assert (i.y < static_cast<int> (img.rows ()));
        for (size_t x = i.x; x < (i.x + i.len); ++x)
        {
            assert (x < img.cols ());
            img (i.y, x) = p;
        }
    }
}

/// @brief get pixel coordinates of a line drawn from p1 to p2
std::vector<point> get_line (const point &p1, const point &p2)
{
    const int p1x = ::round (p1.x);
    const int p1y = ::round (p1.y);
    const int p2x = ::round (p2.x);
    const int p2y = ::round (p2.y);
    const int dx = std::abs (p2x - p1x);
    const int dy = std::abs (p2y - p1y);
    const int sx = (p1x < p2x) ? 1 : -1;
    const int sy = (p1y < p2y) ? 1 : -1;
    int err = dx - dy;
    int x = p1x;
    int y = p1y;
    std::vector<point> p;
    for (;;)
    {
        p.push_back (point (x, y));
        if (x == p2x && y == p2y)
            break;
        double e2 = 2.0 * err;
        if (e2 > -dy)
        {
            err = err - dy;
            x = x + sx;
        }
        if (e2 < dx)
        {
            err = err + dx;
            y = y + sy;
        }
    }
    return p;
}

void draw_line (grayscale8_image_t &img, const point &p1, const point &p2, unsigned p)
{
    std::vector<point> pts = get_line (p1, p2);
    const rect img_rect (0, 0, img.cols (), img.rows ());
    for (auto i : pts)
    {
        int x = ::round (i.x);
        int y = ::round (i.y);
        if (contains (img_rect, x, y))
            img (y, x) = p;
    }
}

void draw_lines (grayscale8_image_t &img, const polygon &poly, unsigned p)
{
    for (size_t i = 0; i < poly.size (); ++i)
        draw_line (img, poly[i], poly[(i + 1) % poly.size ()], p);
}

void draw_line (rgb8_image_t &img, const point &p1, const point &p2, const rgb8_pixel_t &p)
{
    std::vector<point> pts = get_line (p1, p2);
    const rect img_rect (0, 0, img.cols (), img.rows ());
    for (auto i : pts)
    {
        int x = ::round (i.x);
        int y = ::round (i.y);
        if (contains (img_rect, x, y))
        {
            img (y, x, 0) = p[0];
            img (y, x, 1) = p[1];
            img (y, x, 2) = p[2];
        }
    }
}

void draw_lines (rgb8_image_t &img, const polygon &poly, const rgb8_pixel_t &p)
{
    for (size_t i = 0; i < poly.size (); ++i)
        draw_line (img, poly[i], poly[(i + 1) % poly.size ()], p);
}

grayscale8_image_t crop (const grayscale8_image_t &img, const rect &r)
{
    grayscale8_image_t c (r.height, r.width);
    for (size_t i = 0; i < c.rows (); ++i)
    {
        unsigned char *dst = &c (i, 0, 0);
        const unsigned char *src = &img (i + r.y, r.x, 0);
        size_t sz = c.cols () * c.channels ();
        memcpy (dst, src, sz);
    }
    return c;
}

void alpha_blend (const grayscale8_image_t &a, grayscale8_image_t &b, const double alpha)
{
    assert (a.rows () == b.rows ());
    assert (a.cols () == b.cols ());
    assert (alpha <= 1.0);
    assert (alpha >= 0.0);
    for (size_t i = 0; i < a.size (); ++i)
        b[i] = alpha * a[i] + (1 - alpha) * b[i];
}

points create_elliptical_path (
    const unsigned w,
    const unsigned h,
    const double sx = 0.5,
    const double sy = 0.5)
{
    // compute offsets
    const double ox = w / 2.0;
    const double oy = h / 2.0;
    // generate more points than we will need
    points path;
    const size_t N = sx * w * 20 + sy * h * 20;
    for (size_t i = 0; i < N; ++i)
    {
        const double r = 0.5;
        const double theta = i * 360.0 / N;
        double x;
        double y;
        pol2cart (r, deg_to_rad (theta), x, y);
        point p (x * w * sx + ox, y * h * sy + oy);
        if (path.empty () || path.back () != p)
            path.push_back (p);
    }
    return path;
}

} // namespace image_tiler

#endif // GRAPHICS_HPP
