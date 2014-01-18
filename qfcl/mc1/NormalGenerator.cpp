// NormalGenerator.cpp
// A class hierarchy for generating random numbers, 
// vectors and matrices.
// 
// This hierarchy uses the Template Method Pattern and it
// delegates to a Strategy pattern for generating uniform
// random numbers.
//
// The solution is object-oriented and uses run-time polymorphic
// functions. In another chapter we use policy classes and templates.
//
// Last modifucation dates:
//
//	2008-3-6 DD kick off
//  2009-5-16 DD generate fixed arrays of normal variates
//	2009-6-29 DD Boost Normal generator
//	2011-12-9 DD strippded to Boost
//  2011-12-11 DD template version
//
// (C) Datasim Education BV 2008-2011
//

#include "NormalGenerator.hpp"
#include <boost/random.hpp>
#include <cmath>


template <typename Generator>
BoostNormal<Generator>::BoostNormal(const Generator& generator, long N) : 
					rng(Generator()), vec(ublas::vector<double>(N, 0.0))
{

	rng.seed(static_cast<boost::uint32_t> (std::time(0)));
	nor = boost::normal_distribution<>(0.0,1.0);

	myRandom = new boost::variate_generator<Generator&, boost::normal_distribution<> > (rng, nor);
}
template <typename Generator>
void BoostNormal<Generator>::getNormalVector()
{ 

	for(long i=0; i < vec.size();  ++i)
	{
		vec[i] = (*myRandom)();
	}
	
}

template <typename Generator>
double BoostNormal<Generator>::RN() const
{
	return (*myRandom)();
}

template <typename Generator>
BoostNormal<Generator>::~BoostNormal()
{
    std::cerr << "Debug: " << myRandom << std::endl;
	delete myRandom;
}
