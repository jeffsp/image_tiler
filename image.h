/// @file image.h
/// @brief image container adapter
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2014-03-03

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <cassert>
#include <stdexcept>
#include <array>
#include <vector>

namespace image_tiler
{

template<typename T,size_t CHANNELS>
struct pixel
{
    std::vector<T> c;
    pixel () : c (CHANNELS)  { }
    pixel (std::initializer_list<T> l) : c (l)  { }
    // write
    T & operator[] (size_t i) { return c[i]; }
    // read
    const T & operator[] (size_t i) const { return c[i]; }
};

typedef pixel<unsigned char,3> rgb8_pixel_t;

template<typename T, size_t CHANNELS, class Cont = std::vector<T>>
class image
{
    public:
    typedef image<T,CHANNELS,Cont> self_type;
    typedef typename Cont::value_type value_type;
    typedef typename Cont::pointer pointer;
    typedef typename Cont::const_pointer const_pointer;
    typedef typename Cont::reference reference;
    typedef typename Cont::const_reference const_reference;
    typedef typename Cont::size_type size_type;
    typedef typename Cont::difference_type difference_type;
    typedef typename Cont::allocator_type allocator_type;
    typedef typename Cont::iterator iterator;
    typedef typename Cont::const_iterator const_iterator;
    typedef typename Cont::reverse_iterator reverse_iterator;
    typedef typename Cont::const_reverse_iterator const_reverse_iterator;

    /// @brief Default constructor
    explicit image ()
        : rows_ (0), cols_ (0)
    { }
    /// @brief Size constructor
    /// @param rows number of rows in image
    /// @param cols number of columns in image
    /// @param v initialization value
    image (size_t rows, size_t cols, const T &v = T ())
        : rows_ (rows), cols_ (cols), cont_ (rows * cols * CHANNELS, v)
    { }
    /// @brief Copy constructor
    /// @param m image to copy
    image (const self_type &m)
        : rows_ (m.rows_), cols_ (m.cols_), cont_ (m.cont_)
    { }

    /// @brief Get dimensions
    /// @return the number of rows
    size_t rows () const
    { return rows_; }
    /// @brief Get dimensions
    /// @return the number of cols
    size_t cols () const
    { return cols_; }
    /// @brief Get dimensions
    /// @return the number of channels
    size_t channels () const
    { return CHANNELS; }
    /// @brief Get number of elements in the image
    /// @return the total number of elements in the image
    size_t size () const
    { return cont_.size (); }
    /// @brief Indicates if the image has a zero dimension
    /// @return true if the image is empty, otherwise false
    bool empty() const
    { return cont_.empty (); }

    /// @brief Swap this image with another
    void swap (self_type &m)
    {
        std::swap (rows_, m.rows_);
        std::swap (cols_, m.cols_);
        std::swap (cont_, m.cont_);
    }
    /// @brief Copy assignment
    self_type &operator= (const self_type &rhs)
    {
        if (this != &rhs)
        {
            self_type tmp (rhs);
            swap (tmp);
        }
        return *this;
    }
    /// @brief Assign all element values
    /// @param v value to assign
    void assign (const T &v)
    { cont_.assign (cont_.size (), v); }

    /// @brief Element access
    reference front ()
    { return cont_.front (); }
    /// @brief Element access
    const_reference front () const
    { return cont_.front (); }
    /// @brief Element access
    reference back ()
    { return cont_.back (); }
    /// @brief Element access
    const_reference back () const
    { return cont_.back (); }

    /// @brief Random access
    /// @param i element index
    reference operator[] (size_t i)
    {
        assert (i < cont_.size ());
        return *(cont_.begin () + i);
    }
    /// @brief Random access
    /// @param i element index
    const_reference operator[] (size_t i) const
    {
        assert (i < cont_.size ());
        return *(cont_.begin () + i);
    }
    /// @brief Random access
    /// @param r element row
    /// @param c element col
    /// @param k element col
    reference operator() (size_t r, size_t c, size_t k = 0)
    {
        assert (index (r, c, k) < cont_.size ());
        return *(cont_.begin () + index (r, c, k));
    }
    /// @brief Random access
    /// @param r element row
    /// @param c element col
    const_reference operator() (size_t r, size_t c, size_t k = 0) const
    {
        assert (index (r, c, k) < cont_.size ());
        return *(cont_.begin () + index (r, c, k));
    }
    /// @brief Checked random access
    /// @param r element row
    /// @param c element col
    /// @param k element channel
    ///
    /// Throws if the subscript is invalid.
    reference at (size_t r, size_t c, size_t k)
    { return cont_.at (index (r, c, k)); }
    /// @brief Checked random access
    /// @param r element row
    /// @param c element col
    ///
    /// Throws if the subscript is invalid.
    const_reference at (size_t r, size_t c, size_t k) const
    { return cont_.at (index (r, c, k)); }

    /// @brief Iterator access
    iterator begin ()
    { return cont_.begin (); }
    /// @brief Iterator access
    const_iterator begin () const
    { return cont_.begin (); }
    /// @brief Iterator access
    iterator end ()
    { return cont_.end (); }
    /// @brief Iterator access
    const_iterator end () const
    { return cont_.end (); }
    /// @brief Iterator access
    reverse_iterator rbegin ()
    { return cont_.rbegin (); }
    /// @brief Iterator access
    const_reverse_iterator rbegin () const
    { return cont_.rbegin (); }
    /// @brief Iterator access
    reverse_iterator rend ()
    { return cont_.rend (); }
    /// @brief Iterator access
    const_reverse_iterator rend () const
    { return cont_.rend (); }

    /// @brief Get an iterator given a subscript
    /// @param r element row
    /// @param c element col
    /// @param k element channel
    iterator loc (size_t r, size_t c, size_t k)
    { return begin () + index (r, c, k); }
    /// @brief Get an element index given its subscripts
    /// @param r element row
    /// @param c element col
    /// @param k element channel
    size_t index (size_t r, size_t c, size_t k) const
    { return r * cols_ * CHANNELS + c * CHANNELS + k; }

    /// @brief Remove all elements
    void clear ()
    {
        self_type tmp;
        swap (tmp);
    }

    /// @brief Compare two images
    template<typename M,typename C>
    friend bool operator== (const image<M,CHANNELS,C> &a, const image<M,CHANNELS,C> &b);

    private:
    size_t rows_;
    size_t cols_;
    Cont cont_;
};

/// @brief Compare two images
template<typename T,size_t CHANNELS,typename Cont>
inline bool operator== (const image<T,CHANNELS,Cont> &a, const image<T,CHANNELS,Cont> &b)
{
    return a.rows_ == b.rows_ &&
        a.cols_ == b.cols_ &&
        a.cont_ == b.cont_;
}

/// @brief Compare two images
template<typename T,size_t CHANNELS,typename Cont>
inline bool operator!= (const image<T,CHANNELS,Cont> &a, const image<T,CHANNELS,Cont> &b)
{
    return !(a == b);
}

// grayscale
typedef image<unsigned char,1> grayscale8_image_t;

// interlaced RGB
typedef image<unsigned char,3> rgb8_image_t;
typedef image<unsigned char,4> rgba8_image_t;

// planar RGB
typedef std::array<grayscale8_image_t,3> prgb8_image_t;

template<typename T>
grayscale8_image_t get_channel (const T &p, const size_t n)
{
    grayscale8_image_t q (p.rows (), p.cols ());
    for (size_t i = 0; i < p.rows (); ++i)
        for (size_t j = 0; j < p.cols (); ++j)
            q (i, j, 0) = p (i, j, n);
    return q;
}

template<typename T>
void put_channel (const grayscale8_image_t &p, T &q, const size_t n)
{
    for (size_t i = 0; i < p.rows (); ++i)
        for (size_t j = 0; j < p.cols (); ++j)
            q (i, j, n) = p (i, j, 0);
}

} // namespace image_tiler

#endif // IMAGE_HPP
