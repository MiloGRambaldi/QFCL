//  Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
//
//  Use, modification and distribution are subject to the BOOST Software License. 
// (See accompanying file LICENSE.txt)

#ifndef QFCL_RANDOM_CPP_RAND_HPP
#define QFCL_RANDOM_CPP_RAND_HPP

#include <cstdlib>

namespace qfcl {
namespace random {

struct cpp_rand
{
    typedef int result_type;
    static int min() { return 0; }
    static int max() { return RAND_MAX; }
    int operator()() { return rand(); }
};

}} // namespaces
#endif // 
