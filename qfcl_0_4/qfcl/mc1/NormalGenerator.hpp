// NormalGenerator.hpp
//
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
// (C) Datasim Education BV 2008-2011
//

#ifndef NormalGenerator_HPP
#define NormalGenerator_HPP

#include <boost/random.hpp>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>			// Sending to IO stream.

namespace ublas=boost::numeric::ublas;

template <typename Generator>
				class BoostNormal
{
private:
	Generator rng;
	boost::normal_distribution<> nor;

public: // for convenience

	ublas::vector<double> vec;
	boost::variate_generator<Generator&, boost::normal_distribution<> >* myRandom;

public:
	BoostNormal() {}
	BoostNormal(const Generator& generator, long N);	// Generate N N(0,1) numbers
	void getNormalVector();
	double RN() const;

	~BoostNormal();
};



#endif
