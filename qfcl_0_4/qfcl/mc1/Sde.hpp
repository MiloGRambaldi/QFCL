// Sde.hpp
//
// Classes for generic SDEs. 
//
// (C) Datasim Education BV 2003-2011
//

#ifndef SDE_hpp
#define SDE_hpp

#include <boost/function.hpp>
#include "Range.cpp"		// 1d interval [a,b] for Time dimension

template <typename X = double, typename Time = double, typename RT = double>
				class Sde
{ 
public:

	X ic;				// Initial condition
	Range<Time> ran;	// Interval where SDE 'lives'

public: // For convenience and performance

	
	// General function 
	boost::function<RT (X, Time)> drift;
	boost::function<RT (X, Time, X)> driftCorrected;
	boost::function<RT (X, Time)> diffusion;
	boost::function<RT (X, Time)> diffusionDerivative;

	Sde() :	ic(X()), ran(Range<Time>()), 
			drift(boost::function<RT (X, Time)> ()), driftCorrected(boost::function<RT (X, Time, X)> ()), diffusion(boost::function<RT (X, Time)> ()),
			diffusionDerivative(boost::function<RT (X, Time)> ())
	{
	
	}

	Sde(const Sde<X, Time, RT>& sde2) : ic(sde2.ic), ran(sde2.ran),
										drift(sde2.drift), driftCorrected(sde2.driftCorrected), diffusion(sde2.diffusion),
										diffusionDerivative(sde2.diffusionDerivative)
	{
		
	}

	// Can choose to give the derivative of diffusion (for Milstein scheme)
	Sde(double initialCondition, const Range<double>& interval,
			const boost::function<RT (X, Time)>& driftFunction, 
			const boost::function<RT (X, Time, X)>& driftFunctionCorrection, 
			const boost::function<RT (X, Time)>& diffusionFunction, const boost::function<RT (X, Time)>& diffusionFunctionDer)
			: ic(initialCondition), ran(interval), drift(driftFunction), driftCorrected(driftFunctionCorrection),
			  diffusion(diffusionFunction), diffusionDerivative(diffusionFunctionDer)
	{
		// cout << ic << ", " << ran.low() << ", " << ran.high();
	}
};


#endif