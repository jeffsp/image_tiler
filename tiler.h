/// @file tiler.h
/// @brief tiler
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2014-03-07

#ifndef TILER_H
#define TILER_H

#include "geometry.h"

namespace image_tiler
{

/// @brief get locations of tiles in a rectanglar window
///
/// @param row rows in window
/// @param cols cols in window
/// @param origin center point of window
/// @param tile_width tile size
/// @param tile_height tile size
/// @param angle rotation angle in degrees
/// @param is_triangular true if tiles are triangular
///
/// @return container of tile location points
points get_tile_locations (const size_t rows,
    const size_t cols,
    const point &origin,
    const double tile_width,
    const double tile_height,
    const double angle,
    const bool is_triangular)
{
    // get window
    polygon w { point (0.0, 0.0), point (cols, 0.0), point (0.0, rows), point (cols, rows) };
    // convert to tile coordinates
    w = affine (w, negate (origin), -angle, 1.0 / tile_width, 1.0 / tile_height);
    // get points that enclose the window
    rectf r = get_bounding_rectf (w);
    // if the tiles are layed out triangularly, odd numbered rows have an x offset of -0.5
    const double odd_offset = is_triangular ? 0.5 : 0.0;
    // get all points in tile coordinates that overlap with the window
    //
    // also, pad the overlapping rectable by one unit in all directions because tiles don't always cover a complete
    // rectangle
    points p;
    for (double i = floor (r.miny) - 1.0; i < r.maxy + 1.0; i += 1.0)
    {
        bool is_odd = static_cast<int> (abs (floor (i))) & 1;
        for (double j = floor (r.minx) - 1.0 - is_odd * odd_offset; j < r.maxx + 1.0; j += 1.0)
        {
            p.push_back (point (j, i));
        }
    }
    // convert back to window coordinates
    p = affine (p, tile_width, tile_height, angle, origin);
    return p;
}

/// @brief get tiled polygons for the specified locations
///
/// @param tile_locations the tile locations
/// @param polys polygons contained in one tile
/// @param scale scale of the tile
/// @param angle angle of the tile
///
/// @return all the polygons for all the locations
polygons get_tiled_polygons (const points &tile_locations, const polygons &polys, const double scale, const double angle)
{
    polygons all_polys;

    // for each tile location
    for (const auto &offset : tile_locations)
    {
        // for each polygon in a tile
        for (const auto &tile_poly : polys)
        {
            // convert to window coordinates
            // save off a transformed poly
            all_polys.push_back (affine (tile_poly, scale, scale, angle, offset));
        }
    }
    return all_polys;
}

/// @brief get polygons that overlap a window
///
/// @param w width of window
/// @param h height of window
/// @param p polygons
///
/// @return  overlapping polys
///
/// The algorithm is guaranteed to get all overlapping polygons, but it may return some that don't overlap.  The
/// non-overlapping ones' scanlines will get clipped, so the performance hit is minor.
polygons get_overlapping_polygons (const unsigned w, const unsigned h, const polygons &p)
{
    // get the clipping boundary
    const polygon window { point (0, 0), point (w, 0), point (w, h), point (0, h) };
    polygons l;
    // copy to l if they are close enough
    copy_if (p.begin (), p.end (), back_inserter (l),
        [&window] (const polygon &a)
        {
            return is_close (window, a);
        });
    return l;
}

typedef std::vector<scanlines> polygon_scanlines;

/// @brief get raster scanlines associated with some polygons
///
/// @param p polygons
///
/// @return container of container of scanlines
polygon_scanlines get_polygon_scanlines (const polygons &p)
{
    polygon_scanlines ps (p.size ());
    transform (p.begin (), p.end (), ps.begin (),
        [] (const polygon &a)
        {
            return get_convex_polygon_scanlines (a);
        });
    return ps;
}

/// @brief clip polygon scanlines to a window
///
/// @param w width of window
/// @param h height of window
/// @param s input polygon scanlines
///
/// @return clipped scanlines
polygon_scanlines clip_scanlines (const unsigned w, const unsigned h, const polygon_scanlines &s)
{
    // get the clipping boundary
    const rect window { 0, 0, w, h };
    polygon_scanlines ps (s.size ());
    transform (s.begin (), s.end (), ps.begin (),
        [&] (const scanlines &a)
        {
            // clip the scanlines to the window
            //
            // it's OK to have an empty container of scanlines
            return clip (a, window);
        });
    return ps;
}

}

#endif // TILER_H
