/* qfcl/math/simple/functions.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef	QFCL_MATH_SIMPLE_FUNCTIONS_HPP
#define	QFCL_MATH_SIMPLE_FUNCTIONS_HPP

/*! \file qfcl/math/simple/functions.hpp
	\brief simple functions

	\author James Hirschorn
	\date Created May 21, 2012
*/

#include <limits>
#include <cmath>

namespace qfcl {

namespace math {

template<typename T>
inline T square(const T & x)
{
	return x * x;
}

template<typename T>
inline T one()
{
	return static_cast<T>(1);
}

template<typename T>
inline T two()
{
	return static_cast<T>(2);
}

template<typename T>
inline T half()
{
	return one<T>() / two<T>();
}

template<typename T>
inline T epsilon()
{
	return std::numeric_limits<T>::epsilon();
}

template<typename T>
inline bool approx_equal(const T & x, const T & y)
{
	static const int factor = 10;

	return abs(x - y) <= factor * epsilon<T>(); 
}

/// NOTE: Is this precise?
template<typename T>
inline
T pi()
{
    static bool set = false;
    static T _pi;

    if (!set)
    {
        _pi = 2 * std::asin( one<T>() );
        set = true;
    }

    return _pi;
}

}	// namespace math

}	// namespace qfcl

#endif	// QFCL_MATH_SIMPLE_FUNCTIONS_HPP
