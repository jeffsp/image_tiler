/// @file geometry.h
/// @brief geometry
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2014-03-03

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>
#include <iostream>
#include <vector>

namespace image_tiler
{

struct point
{
    point () : x (0.0), y (0.0) { }
    point (const double x, const double y)
        : x (x), y (y) { }
    point operator*= (const double s)
    {
        x *= s;
        y *= s;
        return *this;
    }
    point operator+= (const point &p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }
    point operator-= (const point &p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    }
    double x;
    double y;
};

std::ostream& operator<< (std::ostream &s, const point &p)
{
    s << ' ' << p.x;
    s << ' ' << p.y;
    return s;
}

point operator+ (const point &a, const point &b)
{
    point c (a);
    return c += b;
}

typedef std::vector<point> points;

class polygon
{
    public:
        typedef typename points::iterator iterator;
        typedef typename points::const_iterator const_iterator;
        polygon () { }
        polygon (const points &p) : _points(p) { }
        polygon (size_t sz) : _points(sz) { }
        polygon (const std::initializer_list<point> &l) : _points(l) { }
        point &operator[] (const size_t index) { return _points[index]; }
        const point &operator[] (const size_t index) const { return _points[index]; }
        size_t size() const { return _points.size (); }
        iterator begin () { return _points.begin (); }
        const_iterator begin () const { return _points.begin (); }
        iterator end () { return _points.end (); }
        const_iterator end () const { return _points.end (); }
        bool empty() const { return _points.empty (); }
        point &back () { return _points.back(); }
        const point &back () const { return _points.back(); }
        void push_back (const point &p) { _points.push_back(p); }
        friend std::ostream& operator<< (std::ostream &s, const polygon &p)
        {
            for (auto i : p._points)
                s << i << std::endl;
            return s;
        }
    private:
        points _points;
};

typedef std::vector<polygon> polygons;

std::ostream& operator<< (std::ostream &s, const points &p)
{
    for (auto i : p)
        s << i << std::endl;
    return s;
}

points operator+ (const points &a, const point &b)
{
    points c (a);
    for (auto &i : c)
        i += b;
    return c;
}

std::ostream& operator<< (std::ostream &s, const polygons &p)
{
    for (auto i : p)
        s << i << std::endl;
    return s;
}

bool operator== (const point &a, const point &b)
{
    return a.x == b.x && a.y == b.y;
}

bool operator!= (const point &a, const point &b)
{
    return !(a == b);
}

double distance (const point &a, const point &b)
{
    const double dx = a.x - b.x;
    const double dy = a.y - b.y;
    return sqrt (dx * dx + dy * dy);
}

struct line
{
    line (const point &p1, const point &p2)
        : p1 (p1), p2 (p2) { }
    point p1;
    point p2;
};

point round (const point &p)
{
    return point (::round (p.x), ::round (p.y));
}

constexpr double deg_to_rad (const double x)
{
    return x * M_PI / 180.0;
}

constexpr double rad_to_deg (const double x)
{
    return x * 180.0 / M_PI;
}

point negate (const point &p)
{
    return point (-p.x, -p.y);
}

template<typename T>
T mirrorx (const T &p)
{
    T tmp (p);
    for (size_t i = 0; i < tmp.size (); ++i)
        tmp[i].y = -p[i].y;
    return tmp;
}

template<typename T>
T mirrory (const T &p)
{
    T tmp (p);
    for (size_t i = 0; i < tmp.size (); ++i)
        tmp[i].x = -p[i].x;
    return tmp;
}

template<typename T>
T translate (const T &poly, const point &offset)
{
    T tmp (poly);
    for (size_t i = 0; i < tmp.size (); ++i)
        tmp[i] += offset;
    return tmp;
}

point rotate (const point &p, const double deg)
{
    double r = deg_to_rad (deg);
    return point (p.x * cos (r) - p.y * sin (r), p.x * sin (r) + p.y * cos (r));
}

template<typename T>
T rotate (const T &poly, const double deg)
{
    T tmp (poly);
    for (size_t i = 0; i < tmp.size (); ++i)
        tmp[i] = rotate (poly[i], deg);
    return tmp;
}

template<typename T>
T scale (const T &poly, const double sx, const double sy)
{
    T tmp (poly);
    for (size_t i = 0; i < tmp.size (); ++i)
    {
        tmp[i].x *= sx;
        tmp[i].y *= sy;
    }
    return tmp;
}

template<typename T>
T scale (const T &poly, const double s)
{
    T tmp (poly);
    for (size_t i = 0; i < tmp.size (); ++i)
        tmp[i] *= s;
    return tmp;
}

template<typename T>
T affine (const T &poly, const point &t, const double deg, const double sx, const double sy)
{
    T tmp = translate (poly, t);
    tmp = rotate (tmp, deg);
    tmp = scale (tmp, sx, sy);
    return tmp;
}

template<typename T>
T affine (const T &poly, const double sx, const double sy, const double deg, const point &t)
{
    T tmp = scale (poly, sx, sy);
    tmp = rotate (tmp, deg);
    tmp = translate (tmp, t);
    return tmp;
}

polygon create_regular_polygon (const size_t n, const double outer_angle)
{
    assert (n != 0);
    polygon p (n);
    p[0] = point (0.0, 0.0);
    double th = 0.0;
    for (size_t i = 0; i + 1 < p.size (); ++i)
    {
        double x = p[i].x + cos (deg_to_rad (th));
        double y = p[i].y + sin (deg_to_rad (th));
        p[i + 1] = point (x, y);
        th += outer_angle;
    }
    return p;
}

struct rectf
{
    double minx, miny;
    double maxx, maxy;
};

std::ostream& operator<< (std::ostream &s, const rectf &r)
{
    s << ' ' << r.minx;
    s << ' ' << r.miny;
    s << ' ' << r.maxx;
    s << ' ' << r.maxy;
    return s;
}

// get the rectangular set of points that surround a set of points
rectf get_bounding_rectf (const polygon &p)
{
    rectf r;
    r.minx = std::numeric_limits<double>::max ();
    r.miny = r.minx;
    r.maxx = std::numeric_limits<double>::lowest ();
    r.maxy = r.maxx;
    for (auto i: p)
    {
        if (i.x < r.minx)
            r.minx = i.x;
        if (i.y < r.miny)
            r.miny = i.y;
        if (i.x > r.maxx)
            r.maxx = i.x;
        if (i.y > r.maxy)
            r.maxy = i.y;
    }
    assert (r.minx <= r.maxx);
    assert (r.miny <= r.maxy);
    return r;
}

struct square : public polygon
{
    square () : polygon (create_regular_polygon (4, 90.0)) { }
};

struct octagon : public polygon
{
    octagon () : polygon (create_regular_polygon (8, 45.0)) { }
};

struct triangle90 : public polygon
{
    triangle90 ()
    {
        push_back (point (0.0, 0.0));
        push_back (point (1.0, 0.0));
        push_back (point (1.0, 1.0));
    }
};

struct triangle60 : public polygon
{
    triangle60 () : polygon (create_regular_polygon (3, 120.0)) { }
};

struct hexagon : public polygon
{
    hexagon () : polygon (create_regular_polygon (6, 60.0)) { }
};

struct dodecagon : public polygon
{
    dodecagon () : polygon (create_regular_polygon (12, 30.0)) { }
};

struct triangle135 : public polygon
{
    triangle135 ()
    {
        push_back (point (0.0, 0.0));
        push_back (point (1.0, 0.0));
        push_back (point (0.5f, sqrt (3.0) / 6.0));
    }
};

struct triangle30 : public polygon
{
    triangle30 ()
    {
        push_back (point (0.0, 0.0));
        push_back (point (cos (deg_to_rad (30.0)), 0.0));
        push_back (point (cos (deg_to_rad (30.0)), sin (deg_to_rad (30.0))));
    }
};

struct cairo_pentagon : public polygon
{
    cairo_pentagon ()
    {
        // bottom: 1 short side
        const double A = sqrt (3.0) / 3.0;
        // 4 long sides
        const double B = 0.5 + sqrt (3.0) / 6.0;
        const double DX1 = B * cos (deg_to_rad (60.0));
        const double DY1 = B * sin (deg_to_rad (60.0));
        const double DX2 = B * cos (deg_to_rad (30.0));
        const double DY2 = B * sin (deg_to_rad (30.0));
        push_back (point (0.0, 0.0));
        push_back (point (A, 0.0));
        push_back (point (A + DX1, DY1));
        push_back (point (-DX1 + DX2, DY1 + DY2));
        push_back (point (-DX1, DY1));
    }
};

struct pentagon30 : public polygon
{
    pentagon30 ()
    {
        push_back (point (0.0, 0.0));
        push_back (point (cos (deg_to_rad (60.0)), sin (deg_to_rad (60.0))));
        push_back (point (operator[](1).x - 0.5 * cos (deg_to_rad (60.0)), operator[](1).y + 0.5 * sin (deg_to_rad (60.0))));
        push_back (point (-operator[](2).x, operator[](2).y));
        push_back (point (-operator[](1).x, operator[](1).y));
    }
};

struct rhombus : public polygon
{
    rhombus ()
    {
        const double DX = cos (deg_to_rad (30.0));
        const double DY = sin (deg_to_rad (-30.0));
        push_back (point (0.0, 0.0));
        push_back (point (DX, DY));
        push_back (point (DX + DX, 0.0));
        push_back (point (DX, -DY));
    }
};

struct kite : public polygon
{
    kite ()
    {
        push_back (point (0.0, 0.0));
        push_back (point (cos (deg_to_rad (60.0)), sin (deg_to_rad (60.0))));
        push_back (point (0.0, 2.0 * sqrt (3.0) / 3));
        push_back (point (cos (deg_to_rad (120.0)), sin (deg_to_rad (60.0))));
    }
};

}

#endif
