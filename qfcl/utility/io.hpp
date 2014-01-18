/* qfcl/utility/io.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef	QFCL_UTILITY_IO_HPP
#define QFCL_UTILITY_IO_HPP

/*! \file io.hpp
	\brief Some I/O routines.

	\author James Hirschorn
	\date April 9, 2012
*/

#include <algorithm>
#include <iterator>

namespace qfcl {

namespace io {

//! writes \p n characters, beginning at address \p p, to a character based output iterator \p OutIt
template< typename CharType, typename CharTraits, template<typename, typename> class OutIt >
void write_char_n(void * p, size_t n, OutIt<CharType, CharTraits> & iter)
{
	std::copy_n( reinterpret_cast<CharType *>(p), n, iter );
}

//! reads \p n characters, beginning at address \p p, to a character based input iterator \p InIt
template< typename CharType, typename CharTraits, template<typename, typename> class InIt >
void read_char_n(void * p, size_t n, InIt<CharType, CharTraits> & iter)
{
	// the STL version does not advance the iterator n times ?!
	//std::copy_n( iter, n, reinterpret_cast<CharType *>(p) );
	CharType * re_p = reinterpret_cast<CharType *>(p);
	for (size_t i = 0; i < n; ++i)
		re_p[i] = *(iter++);
}

}	// namespace io

}	// namespace qfcl

#endif	// !QFCL_UTILITY_IO_HPP
