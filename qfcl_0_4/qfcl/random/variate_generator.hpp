//  Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
//
//  Use, modification and distribution are subject to the BOOST Software License. 
// (See accompanying file LICENSE.txt)

#ifndef QFCL_RANDOM_VARIATE_GENERATOR_HPP
#define QFCL_RANDOM_VARIATE_GENERATOR_HPP


namespace qfcl {
namespace random {


template<class Engine, class Distribution >
class variate_generator
{
public:
	typedef	Engine			engine_type;
	typedef Distribution	distribution_type;

    variate_generator(Engine e, Distribution d)
    {
        Engine::UNSUPPORTED_ENGINE_DISTRIBUTION_COMBINATION;
        Distribution::UNSUPPORTED_ENGINE_DISTRIBUTION_COMBINATION;
    }
};

}} // namespaces
#endif //
