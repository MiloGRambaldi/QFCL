// SdeOneFactor.hpp
//
// SDE classes
//
// (C) Datasim Educatiin BV 2011
//

#ifndef Sde_hpp
#define Sde_hpp

#include <algorithm>

namespace OneFactorSDE
{ // SDE module

		// Set II, C = 2.1334 (BS exact, see Haug)
		double r = 0.08;
		double d = 0.0;
		double T = 0.25;
		double K = 65.0;
		double vol = 0.30;
		double beta = 1.0;
		double initialCondition = 60.0;

	// Set IIa, CEV, P = 9.3486 when beta = -3
	/*	double r = 0.05;
		double d = 0.0;
		double T = 0.5;
		double K = 100.0;
		double beta = 1.0;//2.0/3.0;
		double vol = 0.20*pow(100.0, 1-beta);// ??? GET MAGIC answer 7.29  ALWAYS :-(
		double initialCondition = 100.0;*/

		// Set I P = 10.191
	/*	double r = 0.05;
		double d = 0.0;
		double T = 0.5;
		double K = 110.0;
		double beta = 1.0;//2.0/3.0;
		double vol = 0.20;
		double initialCondition = 100.0;	// S0
		*/

		// Set III, Stress test T = 5,  P = 17.6933 when sig = 0.2
	/*	double r = 0.0;
		double d = 0.0;
		double T = 5.0;
		double K = 100.0;
		double vol = 0.2;
		double initialCondition = 100.0;*/

		double drift(double X, double t)
		{ // Drift term
	
			return (r - d)*X; // r - D
		}

		double diffusion(double X, double t)
		{ // Diffusion term
	
			return vol * pow(X,beta);
		}

		double diffusionDerivative(double X, double t)
		{ // Diffusion term derivative d(sig)/dX
	
			if (beta >= 1.0)
			{
				return vol*beta*pow(X, beta - 1.0);
			}
			else 
			{
				return vol*beta/pow(X,1.0-beta);
			}

			return vol;
		}

		double driftCorrected(double X, double t, double B)
		{ // Corrected drift term
	
			return drift(X,t) - B*diffusion(X,t)* diffusionDerivative(X, t);
		}


		double myPayOffFunction(pathType<double> path)//double S)
		{ // Call option

			double S = path[path.size() - 1];
			// Call
			//return max(S - K, 0.0);
			
			// Put
			return std::max(K - S, 0.0);
		}


}

namespace CIRSDE
{
		double a = 0.0025;
		double b = 0.05;
		double sig = 0.1;
		double T = 20.0;
		double initialCondition = 0.04;

		double drift(double r, double t)
		{ 
			return a - b*r;
		}

		double diffusion(double r, double t)
		{ 

			return sig*sqrt(r);		
		}


		double diffusionDerivative(double r, double t)
		{ // Diffusion term derivative d(sig)/dr
	
		
			return sig*0.5/sqrt(r);
		}

		double driftCorrected(double r, double t, double B)
		{ // Corrected drift term
	
			return drift(r,t) - B*diffusion(r,t)* diffusionDerivative(r, t);
		}


		double myPayOffFunction(double r)
		{ // Call option

			return 1.0;
		}

} // End CIR

#endif