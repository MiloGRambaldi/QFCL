//  Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
//
//  Use, modification and distribution are subject to the BOOST Software License. 
// (See accompanying file LICENSE.txt)

#ifndef QFCL_RANDOM_UNIFORM_0EX_1EX_HPP
#define QFCL_RANDOM_UNIFORM_0EX_1EX_HPP

#include <qfcl/random/variate_generator.hpp>

namespace qfcl {
namespace random {

template<class RealType = double>
struct uniform_0ex_1ex { typedef RealType result_type; };


template<class Engine, class RealType >
class variate_generator<Engine, uniform_0ex_1ex<RealType> >
{
public:
    typedef Engine                      engine_type;
    typedef uniform_0ex_1ex<RealType>   distribution_type;
    typedef RealType                    result_type;

    // constructor
    // if, min==1, max==3 then  [ 0.5/3, 1.5/3, 2.5/3 ]
    variate_generator(engine_type e, distribution_type d) : _eng(e), _dist(d)
    { 
        _factor = result_type( (_eng.max)() - (_eng.min)() );
        _factor += 1;
        _factor = 1/_factor;
    }
    
    // todo: can we store  "(_eng.min)() - 0.5" ?
    result_type operator()() 
    { return result_type( _eng() - (_eng.min)() + 0.5 ) * _factor; } 

private:
    engine_type         _eng;
    distribution_type   _dist;
    result_type         _factor;
};

}} // namespaces
#endif