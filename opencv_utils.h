/// @file opencv_utils.h
/// @brief opencv utilities
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-12-18

#ifndef OPENCV_UTILS_H
#define OPENCV_UTILS_H

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sys/time.h>

#include "image.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace image_tiler
{

enum class keys
{
    esc,
    space,
    tab,
    up,
    down,
    left,
    right,
    unknown,
    a,
    b,
    c,
    d,
    f,
    l,
    q,
    r,
    s,
    t,
    u,
    v,
    none
};

keys get_key ()
{
    int c = cv::waitKey (1);
    if (c != -1)
        std::clog << "key code: " << c << std::endl;
    switch (c)
    {
        default: return keys::unknown;
        case -1: return keys::none;
        case 27: return keys::esc;
        case 32: return keys::space;
        case 9: return keys::tab;
        case 'a': return keys::a;
        case 'b': return keys::b;
        case 'c': return keys::c;
        case 'd': return keys::d;
        case 'f': return keys::f;
        case 'l': return keys::l;
        case 'q': return keys::q;
        case 'r': return keys::r;
        case 's': return keys::s;
        case 'u': return keys::u;
        case 't': return keys::t;
        case 'v': return keys::v;
        case 65361: return keys::left;
        case 65363: return keys::right;
        case 65362: return keys::up;
        case 65364: return keys::down;
    }
}

cv::Mat image_to_mat (const rgb8_image_t &img)
{
    cv::Mat m (img.rows (), img.cols (), CV_8UC3, cv::Scalar (256, 256, 256));
    for (size_t i = 0; i < img.rows (); ++i)
    {
        for (size_t j = 0; j < img.cols (); ++j)
        {
            // Mat is BGR, not RGB
            m.at<cv::Vec3b> (i, j)[2] = img (i, j, 0);
            m.at<cv::Vec3b> (i, j)[1] = img (i, j, 1);
            m.at<cv::Vec3b> (i, j)[0] = img (i, j, 2);
        }
    }
    return m;
}

rgb8_image_t mat_to_image (const cv::Mat &m)
{
    rgb8_image_t img (m.rows, m.cols);
    for (size_t i = 0; i < img.rows (); ++i)
    {
        for (size_t j = 0; j < img.cols (); ++j)
        {
            // Mat is BGR, not RGB
            img (i, j, 0) = m.at<cv::Vec3b> (i, j)[2];
            img (i, j, 1) = m.at<cv::Vec3b> (i, j)[1];
            img (i, j, 2) = m.at<cv::Vec3b> (i, j)[0];
        }
    }
    return img;
}

rgb8_image_t resize (const rgb8_image_t &img, size_t rows, size_t cols)
{
    cv::Mat m = image_to_mat (img);
    cv::resize (m, m, cv::Size (cols, rows));
    return mat_to_image (m);
}

rgb8_image_t read_image (const std::string &fn)
{
    cv::Mat m = cv::imread (fn);
    if (m.depth () != CV_8U || m.channels () != 3)
        throw std::runtime_error ("image is not 8 bit RGB");
    return mat_to_image (m);
}

}

#endif
