// FdmVisitor.cpp
//
// Simple class to hold finite difference schemes. In
// this version we work with doubles for convenience.
//
// 2007-1-19 DD hpp and cpp
// ...
// 2011-12-9 DD Boost version + stripped down
// 2011-12-11 DD generic RMG class from Boost
// 2011-12-16 DD Richardson extrapolation for Euler
// 2011-12-27 DD PC, Milstein
// 2011-12-28 DD Karhunen-Loeve
// 2011-12-30 DD Predictor-Corrector KL
// 2011-12-30 PC classic, PC adapted using midpoint rather than averaged approach taken 
// most PC methods.
// 2012-1-7 DD moment matching + type II Euler
// 2012-1-9 DD MC102 frozen.
// 2012-3-18 DD index TYPE for loops is now std::size_t; this removes warnings during compilation.
//
// (C) Datasim Education BV 2007-2011
//

#ifndef FDMVisitor_cpp
#define FDMVisitor_cpp

#include "FDMVisitor.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

#include <omp.h>

#include <qfcl/math/simple/functions.hpp>

template <typename X, typename Time, typename RT,typename Generator>
FdmVisitor<X,Time,RT,Generator>::FdmVisitor(long NSteps, const Sde<X,Time,RT>& mySde, const Generator& myGenerator) : 
				sde(mySde), generator(BoostNormal<Generator>(myGenerator, NSteps + 1))
{
		T = sde.ran.high();
		k = mySde.ran.spread()/ Time (NSteps);
		sqrk = sqrt(k);
		x = mySde.ran.mesh(NSteps);
		
		res = pathType<X>(NSteps + 1, 0.0);
			//boost::numeric::ublas::vector<X>(NSteps + 1, 0.0);
	
		N = NSteps;


}

template <typename X, typename Time, typename RT,typename Generator >
//boost::numeric::ublas::vector<X> 
pathType<X> & FdmVisitor<X,Time,RT,Generator>::path() 
{

		// Compute the path
        this -> Visit(sde);

		return res;
}

// Euler
template <typename X, typename Time, typename RT,typename Generator>
ExplicitEuler<X,Time,RT,Generator>::ExplicitEuler(long NSteps, Sde<X,Time,RT>& sde,const Generator& generator)
			: FdmVisitor<X,Time,RT,Generator>(NSteps, sde, generator)
{
			
}

template <typename X, typename Time, typename RT,typename Generator >
void ExplicitEuler<X,Time,RT,Generator>::Visit(Sde<X,Time,RT>& sde)
{

        auto VOld = sde.ic;
	
        res[0] = VOld;
        for (std::size_t index = 1; index < x.size(); ++index)
		{
			time = x[index-1];
            res[index] = VOld  + k * sde.drift(VOld, time)
							+ sqrk * sde.diffusion(VOld, time) *  generator.RN();
		}
}

// Euler, Type II
template <typename X, typename Time, typename RT,typename Generator>
ExplicitEulerTypeII<X,Time,RT,Generator>::ExplicitEulerTypeII(long NSteps, Sde<X,Time,RT>& sde,const Generator& generator)
			: FdmVisitor<X,Time,RT,Generator>(NSteps, sde, generator)
{
		dW2 = boost::numeric::ublas::vector<Time> (N + 1);	
}

template <typename X, typename Time, typename RT,typename Generator >
void ExplicitEulerTypeII<X,Time,RT,Generator>::Visit(Sde<X,Time,RT>& sde)
{
		for (std::size_t j = 0; j < dW2.size(); ++j)
		{
			dW2[j] = generator.RN();
		}


        auto VOld = sde.ic;
	
		res[0] = VOld;
		for (std::size_t index = 1; index < x.size(); ++index)
		{
			time = x[index-1];
            res[index] = VOld  + k * sde.drift(VOld, time)
							//+ sqrk * sde.diffusion(VOld, time) *  generator.RN();
							+ sqrk * sde.diffusion(VOld, time) *  dW2[index];
		}
}

// Euler, Moment matching
template <typename X, typename Time, typename RT,typename Generator>
ExplicitEulerMM<X,Time,RT,Generator>::ExplicitEulerMM(long NSteps, Sde<X,Time,RT>& sde,const Generator& generator)
			: FdmVisitor<X,Time,RT,Generator>(NSteps, sde, generator)
{
		dW2 = boost::numeric::ublas::vector<Time> (N + 1);	
}

template <typename X, typename Time, typename RT,typename Generator >
void ExplicitEulerMM<X,Time,RT,Generator>::Visit(Sde<X,Time,RT>& sde)
{
	// For some reason does not give good results...............
		// Optimise later
		X avg = 0.0;
		for (std::size_t j = 0; j < dW2.size(); ++j)
		{
			dW2[j] = generator.RN();
			avg += dW2[j];
		}
		avg /= X(dW2.size());
		
		for (std::size_t j = 0; j < dW2.size(); ++j)
		{
			dW2[j] -= avg;
		}

        auto VOld = sde.ic;
	
		res[0] = VOld;
		for (std::size_t index = 1; index < x.size(); ++index)
		{
			time = x[index-1];
            res[index] = VOld  + k * sde.drift(VOld, time)
							+ sqrk * sde.diffusion(VOld, time) *  dW2[index];
		}
		
}


// Adapted Predictor-Corrector
template <typename X, typename Time, typename RT,typename Generator>
PredictorCorrector<X,Time,RT,Generator>::PredictorCorrector(long NSteps, Sde<X,Time,RT>& sde,const Generator& generator,X alpha, X beta)
			: FdmVisitor<X,Time,RT,Generator>(NSteps, sde, generator)
{
	
	A = alpha;
	B = beta;
}

template <typename X, typename Time, typename RT,typename Generator >
void PredictorCorrector<X,Time,RT,Generator>::Visit(Sde<X,Time,RT>& sde)
{
	
        auto VOld = sde.ic;
		res[0] = VOld;

		double adjDriftTerm, diffusionTerm, Wincr; // temp variables, readability
		for (std::size_t index = 1; index < x.size(); ++index)
		{
			Wincr = sqrk*generator.RN();
			// Predictor part; we use Euler with 'normal' drift function
			VMid = VOld  + k * sde.drift(VOld, x[index-1]) + sde.diffusion(VOld, x[index-1]) *  Wincr;

			// Corrector part
			/*adjDriftTerm = k * (A * sde.driftCorrected(VMid, x[index], B)
									 + (1.0 - A) * sde.driftCorrected(VOld, x[index-1], B));*/
						
			// Midpoint average
			adjDriftTerm = k*sde.driftCorrected(A*VMid + (1.0 - A)*VOld , 0.5*(x[index] + x[index-1]), B);

			/*diffusionTerm = (B * sde.diffusion(VMid, x[index]) 
								+ (1.0 - B) * sde.diffusion(VOld, x[index-1]) ) * Wincr;*/
			// Midpoint average
			diffusionTerm = sde.diffusion(B*VMid + (1.0 - B)*VOld , 0.5*(x[index] + x[index-1]))* Wincr;

            res[index] = VOld + adjDriftTerm + diffusionTerm;
		}

}

// Standard Predictor-Corrector
template <typename X, typename Time, typename RT,typename Generator>
PredictorCorrectorClassico<X,Time,RT,Generator>::PredictorCorrectorClassico(long NSteps, Sde<X,Time,RT>& sde,const Generator& generator,X alpha, X beta)
			: FdmVisitor<X,Time,RT,Generator>(NSteps, sde, generator)
{
	
	A = alpha;
	B = beta;
}

template <typename X, typename Time, typename RT,typename Generator >
void PredictorCorrectorClassico<X,Time,RT,Generator>::Visit(Sde<X,Time,RT>& sde)
{
	
        auto VOld = sde.ic;
		res[0] = VOld;

		double driftTerm, diffusionTerm, Wincr; // temp variables, readability
		for (std::size_t index = 1; index < x.size(); ++index)
		{
			Wincr = sqrk*generator.RN();
			// Predictor part; we use Euler with 'normal' drift function
			VMid = VOld  + k * sde.drift(VOld, x[index-1]) + sde.diffusion(VOld, x[index-1]) *  Wincr;

			// Corrector part
			/*adjDriftTerm = k * (A * sde.drift(VMid, x[index])
									 + (1.0 - A) * sde.drift(VOld, x[index-1]));*/
			// Midpoint average
			driftTerm = k*sde.drift(A*VMid + (1.0 - A)*VOld , 0.5*(x[index] + x[index-1]));

			/*diffusionTerm = (B * sde.diffusion(VMid, x[index]) 
								+ (1.0 - B) * sde.diffusion(VOld, x[index-1]) ) * Wincr;*/

			// Midpoint average
			diffusionTerm = sde.diffusion(B*VMid + (1.0 - B)*VOld , 0.5*(x[index] + x[index-1]))* Wincr;

            res[index] = VOld + driftTerm + diffusionTerm;

		}
	
}

// Richardson Euler
template <typename X, typename Time, typename RT,typename Generator>
RichardsonEuler<X,Time,RT,Generator>::RichardsonEuler(long NSteps, Sde<X,Time,RT>& sde,const Generator& generator)
			: FdmVisitor<X,Time,RT,Generator>(NSteps, sde, generator)
{
		res2 = boost::numeric::ublas::vector<Time>(2*N + 1);
		x2 = sde.ran.mesh(2*N);
		dW2 = boost::numeric::ublas::vector<Time> (2*N + 1);
		k2 = k * 0.5;
		sqrk2 = sqrt(k2);
}

template <typename X, typename Time, typename RT,typename Generator >
void RichardsonEuler<X,Time,RT,Generator>::Visit(Sde<X,Time,RT>& sde)
{
		
		for (std::size_t j = 0; j < dW2.size(); ++j)
		{
			dW2[j] = generator.RN();
		}

		VOld = sde.ic;
		res2[0] = VOld;
		for (std::size_t index = 1; index < res2.size(); ++index)
		{
			VNew = VOld + k2 * sde.drift(VOld, x2[index-1])
						+ sqrk2 * sde.diffusion(VOld, x2[index-1]) * dW2[index];
			
			res2[index] = VNew;
			
			VOld = VNew;
		}	

		VOld = sde.ic;
		res[0] = VOld;

		double normalRV;
		//double tmp; // avoids warning

		for (std::size_t index = 1; index < res.size(); ++index)
		{
		//	tmp = static_cast<double>(index);
		//	normalRV = (dW2[2.0*tmp] + dW2[2.0*tmp - 1])/sqrt(2.0);
			normalRV = (dW2[2 * index] + dW2[2 * index - 1]) / sqrt(2.0);

			VNew = VOld  + k * sde.drift(VOld, x[index-1])
							+ sqrk * sde.diffusion(VOld, x[index-1]) * normalRV;

			res[index] = VNew;
			VOld = VNew;
		}	

		
	// Extrapolated solution
	for (std::size_t i = 1; i < res.size(); ++i)
	{
		res[i] = 2.0*res2[2*i] - res[i];
	}
	
}

// Milstein
template <typename X, typename Time, typename RT,typename Generator>
Milstein<X,Time,RT,Generator>::Milstein(long NSteps, Sde<X,Time,RT>& sde,const Generator& generator)
			: FdmVisitor<X,Time,RT,Generator>(NSteps, sde, generator)
{
			
}

template <typename X, typename Time, typename RT,typename Generator >
void Milstein<X,Time,RT,Generator>::Visit(Sde<X,Time,RT>& sde)
{

        auto VOld = sde.ic;
		res[0] = VOld;
	
		double Wincr, diffTerm;

		for (std::size_t index = 1; index < x.size(); ++index)
		{
			Wincr = generator.RN();
			diffTerm = sde.diffusion(VOld, x[index-1]);
			
			
			// Explicit Euler part
            res[index] = VOld  + k * sde.drift(VOld, x[index-1])
							+ sqrk * sde.diffusion(VOld, x[index-1]) *  Wincr
						+ 0.5 * diffTerm* sde.diffusionDerivative(VOld, x[index-1])*k*(Wincr*Wincr - 1.0); // 'Correction' part
		}	
}

// KarhunenLoeve 

template <typename X, typename Time, typename RT,typename Generator>
KarhunenLoeve<X,Time,RT,Generator>::KarhunenLoeve(long NSteps, Sde<X,Time,RT>& sde,const Generator& generator, double tolerance)
			: FdmVisitor<X,Time,RT,Generator>(NSteps, sde, generator), tol(tolerance)
{
	Ntrunc = static_cast<long>(sqrt(2.0*sde.ran.high())/(tol*3.1415) - 0.5);
	sqrT = 2.0*sqrt(2.0*T);
	cout << "Truncation Value: " << Ntrunc;
	dW2 = boost::numeric::ublas::vector<Time> (Ntrunc+1); 		
}




template <typename X, typename Time, typename RT,typename Generator>
X KarhunenLoeve<X,Time,RT,Generator>::orthogonalFunction(Time t, long n)
{
	
    X f = (2.0*n + 1.0) * qfcl::math::pi<X>();
	
    return (sqrT/f)*sin(qfcl::math::half<T>()*f*t/T);
	//return (sqrT/f)*mySin<X>(0.5*f*t/T);
}

template <typename X, typename Time, typename RT,typename Generator>
X KarhunenLoeve<X,Time,RT,Generator>::KLExpansion(Time s, Time t) // s > t
{
	
		X value = 0.0;
#pragma omp parallel for reduction (+:value)
		for (std::size_t n = 0; n < dW2.size(); ++n)
		{
			value += dW2[n]*(orthogonalFunction(s, n) - orthogonalFunction(t, n) );
		}

		return value;
}


template <typename X, typename Time, typename RT,typename Generator >
void KarhunenLoeve<X,Time,RT,Generator>::Visit(Sde<X,Time,RT>& sde)
{

		// Generate once for each draw/simulation
		for (std::size_t j = 0; j < dW2.size(); ++j)
		{
			dW2[j] = generator.RN();
		}

        auto VOld = sde.ic;
	
		res[0] = VOld;
		for (std::size_t index = 1; index < x.size(); ++index)
		{
            res[index] = VOld  + k * sde.drift(VOld, x[index-1])
								+ sqrk * sde.diffusion(VOld, time) *  generator.RN(); // Classic Euler
							//	+ sde.diffusion(VOld, time) *  KLExpansion(x[index], x[index-1]);
		}
}


// Predictor-Corrector with Karhune-Loeve
template <typename X, typename Time, typename RT,typename Generator>
PredictorCorrectorKL<X,Time,RT,Generator>::PredictorCorrectorKL(long NSteps, Sde<X,Time,RT>& sde,const Generator& generator,X alpha, X beta, double tolerance)
			: FdmVisitor<X,Time,RT,Generator>(NSteps, sde, generator), tol(tolerance)
{
	
	A = alpha;
	B = beta;

	Ntrunc = static_cast<long>(sqrt(2.0*sde.ran.high())/(tol*3.1415) - 0.5);
	sqrT = 2.0*sqrt(2.0*T);
	cout << "trunc " << Ntrunc;
	dW2 = boost::numeric::ublas::vector<Time> (Ntrunc+1); 		

}

template <typename X, typename Time, typename RT,typename Generator >
void PredictorCorrectorKL<X,Time,RT,Generator>::Visit(Sde<X,Time,RT>& sde)
{
	
		// Generate once for each draw/simulation
		for (std::size_t j = 0; j < dW2.size(); ++j)
		{
			dW2[j] = generator.RN();
		}

        auto VOld = sde.ic;
		res[0] = VOld;

		double adjDriftTerm, diffusionTerm; // temp variables, readability
		for (std::size_t index = 1; index < x.size(); ++index)
		{
		
			// Predictor part; we use Euler with 'normal' drift function
			VMid = VOld  + k * sde.drift(VOld, x[index-1]) + sde.diffusion(VOld, x[index-1]) *  KLExpansion(x[index], x[index-1]);

			// Corrector part
			// Trapezoid average
			/*adjDriftTerm = k * (A * sde.driftCorrected(VMid, x[index], B)
									 + (1.0 - A) * sde.driftCorrected(VOld, x[index-1], B));*/
			// Midpoint average
			adjDriftTerm = k*sde.driftCorrected(A*VMid + (1.0 - A)*VOld , 0.5*(x[index] + x[index-1]), B);
			
			//diffusionTerm = (B * sde.diffusion(VMid, x[index]) 
				//				+ (1.0 - B) * sde.diffusion(VOld, x[index-1]) ) * KLExpansion(x[index], x[index-1]);

			// Midpoint average
			diffusionTerm = sde.diffusion(B*VMid + (1.0 - B)*VOld , 0.5*(x[index] + x[index-1]))* KLExpansion(x[index], x[index-1]);

            res[index] = VOld + adjDriftTerm + diffusionTerm;
		}	
	
}
template <typename X, typename Time, typename RT,typename Generator>
X PredictorCorrectorKL<X,Time,RT,Generator>::orthogonalFunction(Time t, long n)
{
	const double PI = 3.1415;

	X f = (2.0*n + 1.0)*PI;
	
	return (sqrT/f)*sin(0.5*f*t/T);
}

template <typename X, typename Time, typename RT,typename Generator>
X PredictorCorrectorKL<X,Time,RT,Generator>::KLExpansion(Time s, Time t) // s > t
{
	
		X value = 0.0;
#pragma omp parallel for reduction (+:value)
		for (std::size_t n = 0; n < dW2.size(); ++n)
		{
			value += dW2[n]*(orthogonalFunction(s, n) - orthogonalFunction(t, n) );
		}

		return value;
}
#endif
