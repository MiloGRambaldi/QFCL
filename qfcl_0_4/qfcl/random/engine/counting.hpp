/* qfcl/random/engine/counting.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_COUNTING_HPP
#define QFCL_RANDOM_COUNTING_HPP

/*! \file qfcl/random/engine/counting.hpp
	\brief Simple "random" generator that counts through the entire range of its UIntType.

	Roughly based on <tt>class counting</tt> from \c boost/libs/random/performance/random_speed.hpp from \c boost 1.49.0.
	Useful for testing performance overhead.

	\author James Hirschorn
	\date March 22, 2012
*/

#include <iostream>
#include <limits>

#include <boost/cstdint.hpp>
#include <boost/mpl/string.hpp>

#include <qfcl/types.hpp>
#include <qfcl/utility/tmp.hpp>

//#include "reverse_adapter.hpp"

namespace qfcl {

namespace random {

/*! \ingroup random
	@{
*/

//! simplest "random" number generator possible, to check on overhead
template<typename UIntType, typename Name>
class counting
{
public:
  typedef UIntType result_type;

  counting() : _x( static_cast<result_type>(-1) ) { }
  result_type operator()() { return ++_x; }

  static result_type min() { return 0; }
  static result_type max() { return std::numeric_limits<result_type>::max(); }

  void seed(result_type s) { _x = s; }

  typedef Name name;
private:
  result_type _x;
};

namespace detail {

// alias
namespace mpl = boost::mpl;

typedef mpl::string<'c','o','u','n','t','i','n','g'>::type _counting_prefix;
typedef mpl::string<'_','u', 'i', 'n', 't'>::type uint_suffix;

typedef qfcl::tmp::concatenate<_counting_prefix, uint_suffix>::type counting_uint_name;

}	// namespace detail


typedef counting<unsigned int, detail::counting_uint_name> counting_uint;

//! @}

}	// namespace random

}	// namespace qfcl

#endif	// QFCL_RANDOM_COUNTING_HPP
