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

polygons get_tiled_polygons (const points &tile_locations, const polygons &polys, const double angle)
{
    polygons all_polys;

    // for each tile location
    for (auto offset : tile_locations)
    {
        // get a copy of each polygon
        for (auto tile_poly : polys)
        {
            // convert to window coordinates
            // save off the transformed poly
            all_polys.push_back (affine (tile_poly, 1.0, 1.0, angle, offset));
        }
    }
    return all_polys;
}

/*
void tile_image (prgb8_image_t &img, const tiling_parameters &tp)
{
    // get the tile
    const tile &t = tp.get_tile ();

    // normalize by the scale of a tile
    const double scale = tp.get_scale () / t.get_width ();

    // get untranslated tile polygons
    const polygons &tile_polys = t.get_polygons ();

    // get the locations
    points locs = get_tile_locations (img[0].rows (),
        img[0].cols (),
        point (tp.get_center_offset ()),
        scale * t.get_width (),
        scale * t.get_height (),
        tp.get_angle (),
        t.is_triangular ());

    // get the clipping boundary
    const rect window_rect (0, 0, img[0].cols (), img[0].rows ());

    // for each location
    for (auto l : locs)
    {
        // windowed polygons
        polygons window_polys;

        // for each tile polygon
        for (auto p : tile_polys)
        {
            // convert to window coordinates
            auto wp = affine (p, scale, scale, tp.get_angle (), l);

            // if the window contains any point of the windowed polygon, keep it
            if (contains (window_rect, wp))
                window_polys.push_back (wp);
        }

        // for each unclipped windowed polygon
        for (auto wp : window_polys)
        {
            // get all the clipped scanlines
            scanlines s = get_convex_polygon_scanlines (wp);
            // scanlines should already be in ascending order for good cache usage
            // clip the scanlines to the window
            s = clip (s, window_rect);
            // continue if there is nothing to do
            if (s.empty ())
                continue;
            // draw the scanlines
            for (auto channel : { 0, 1, 2 })
            {
                // draw the tiles
                unsigned m = get_mean (img[channel], s);
                fill (s, img[channel], m);
                // draw lines, if needed
                if (tp.get_draw_line ())
                    for (size_t i = 0; i < wp.size (); ++i)
                        draw_lines (img[channel], wp, 255);
            }
        }
    }
}
*/

}

#endif // TILER_H
