/* qfcl/random/engine/numberline.hpp
 *
 * Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_NUMBERLINE_HPP
#define QFCL_RANDOM_NUMBERLINE_HPP

/*! \file qfcl/random/engine/numberline.hpp
	\brief Simple "random" generator that counts throught [MIN, MAX]

	\author M.A. (Thijs) van den Berg, James Hirschorn
	\date August 8, 2012
*/

#include "named_adapter.hpp"

namespace qfcl {
namespace random {


template <int MIN=1, int MAX=10>
struct numberline
{
private:
    int _state;
public:
	typedef int result_type;

    numberline() : _state(MAX) {};
    
    static int min() {return MIN; }
    static int max() {return MAX; }

	void seed(result_type s) { _state = s; }
    
    int operator()() 
    { 
        (_state==MAX) ? _state=MIN : ++_state;
        return _state;
    }
};

// named version of numberline<>

typedef boost::mpl::string<'N', 'u', 'm', 'b', 'e', 'r'>::type Number;
typedef boost::mpl::string<'l', 'i', 'n', 'e'>::type line;
typedef qfcl::tmp::concatenate<Number, line>::type Numberline_name;
typedef qfcl::named_adapter< qfcl::random::numberline<>, Numberline_name > Numberline;

}} // namespaces
#endif // 
