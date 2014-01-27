//  Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
//
//  Use, modification and distribution are subject to the BOOST Software License. 
// (See accompanying file LICENSE.txt)

#include "qfcl/random/engine/cpp_rand.hpp"
#include "qfcl/random/distribution/normal_box_muller_polar.hpp"
#include "qfcl/random/distribution/normal_box_muller.hpp"
#include "qfcl/random/distribution/normal_inversion.hpp"

#include <iostream>

int main()
{
    typedef qfcl::random::cpp_rand ENG;
    typedef qfcl::random::normal_box_muller_polar<> DIST1;
    typedef qfcl::random::normal_box_muller<> DIST2;
    typedef qfcl::random::normal_inversion<> DIST3;

    ENG eng;
    DIST1 dist1;
    DIST2 dist2;
    DIST3 dist3;
    
    
    qfcl::random::variate_generator< ENG, DIST1 > rng1(eng, dist1);
    qfcl::random::variate_generator< ENG, DIST2 > rng2(eng, dist2);
    qfcl::random::variate_generator< ENG, DIST3 > rng3(eng, dist3);
    
    for (int i=0; i<10; ++i) {
        std::cout << rng1() << " " << rng2() << " " << rng3() << std::endl;
    }

    return 0;
}