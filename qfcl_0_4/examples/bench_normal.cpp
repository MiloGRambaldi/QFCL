//  Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
//
//  Use, modification and distribution are subject to the BOOST Software License. 
// (See accompanying file LICENSE.txt)

#include "qfcl/random/engine/cpp_rand.hpp"
#include "qfcl/random/distribution/normal_box_muller_polar.hpp"
#include "qfcl/random/distribution/normal_box_muller.hpp"
#include "qfcl/random/distribution/normal_inversion.hpp"
#include "qfcl/random/distribution/uniform_0in_1in.hpp"

#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <iostream>

int main()
{
    long N = 100*1000*1000;

    {
        typedef qfcl::random::cpp_rand ENG;
        typedef boost::normal_distribution<> DIST;
    
        ENG eng;
        DIST dist;
    
        boost::variate_generator< ENG, DIST > rng(eng, dist);

        double sum = 0;
        boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time() );
        for (long i=0; i<N; ++i)
            sum += rng();
        boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time() );
        boost::posix_time::time_duration duration( time_end - time_start );
        double dt = 0.001* duration.total_milliseconds();
        std::cout << "boost::normal_distribution: " << dt << " sec" << std::endl;
    }

    {
        typedef qfcl::random::cpp_rand ENG;
        typedef qfcl::random::uniform_0in_1in<> DIST;
    
        ENG eng;
        DIST dist;
    
        qfcl::random::variate_generator< ENG, DIST > rng(eng, dist);

        double sum = 0;
        boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time() );
        for (long i=0; i<N; ++i)
            sum += rng();
        boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time() );
        boost::posix_time::time_duration duration( time_end - time_start );
        double dt = 0.001* duration.total_milliseconds();
        std::cout << "qfcl::uniform_0in_1in: " << dt << " sec" << std::endl;
    }
    
    {
        typedef qfcl::random::cpp_rand ENG;
        typedef qfcl::random::normal_box_muller_polar<> DIST;
    
        ENG eng;
        DIST dist;
    
        qfcl::random::variate_generator< ENG, DIST > rng(eng, dist);

        double sum = 0;
        boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time() );
        for (long i=0; i<N; ++i)
            sum += rng();
        boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time() );
        boost::posix_time::time_duration duration( time_end - time_start );
        double dt = 0.001* duration.total_milliseconds();
        std::cout << "qfcl::normal_box_muller_polar: " << dt << " sec" << std::endl;
    }


    {
        typedef qfcl::random::cpp_rand ENG;
        typedef qfcl::random::normal_box_muller<> DIST;
    
        ENG eng;
        DIST dist;
    
        qfcl::random::variate_generator< ENG, DIST > rng(eng, dist);

        double sum = 0;
        boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time() );
        for (long i=0; i<N; ++i)
            sum += rng();
        boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time() );
        boost::posix_time::time_duration duration( time_end - time_start );
        double dt = 0.001* duration.total_milliseconds();
        std::cout << "qfcl::normal_box_muller: " << dt << " sec" << std::endl;

    }

    {
        typedef qfcl::random::cpp_rand ENG;
        typedef qfcl::random::normal_inversion DIST;
    
        ENG eng;
        DIST dist;
    
        qfcl::random::variate_generator< ENG, DIST > rng(eng, dist);

        double sum = 0;
        boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time() );
        for (long i=0; i<N; ++i)
            sum += rng();
        boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time() );
        boost::posix_time::time_duration duration( time_end - time_start );
        double dt = 0.001* duration.total_milliseconds();
        std::cout << "qfcl::normal_inversion: " << dt << " sec" << std::endl;

    }
    
    return 0;
}