// MCAntitheticVariate.cpp
//
// Monte Carlo Method WITHOUT antithetic variates.
//
// (C) Datasim Education BC 2008-2011
//

#include "OptionData.hpp" 
#include "NormalGenerator.hpp"
#include "Range.cpp"
#include <iostream>
#include <cmath>
using namespace std;
#include <boost/timer.hpp>

namespace SDEDefinition
{ // Defines drift + diffusion + data

	OptionData* data;				// The data for the option MC

	double drift(double t, double X)
	{ // Drift term
	
		return (data->r)*X; // r - D
	}

	
	double diffusion(double t, double X)
	{ // Diffusion term
	
		double betaCEV = 1.0;
		return data->sig * pow(X, betaCEV);
		
	}

	double diffusionDerivative(double t, double X)
	{ // Diffusion term, needed for the Milstein method
	
		double betaCEV = 1.0;
		return 0.5 * (data->sig) * (betaCEV) * pow(X, 2.0 * betaCEV - 1.0);
	}
} // End of namespace


int main()
{
	cout <<  "1 factor MC with explicit Euler\n";
	OptionData myOption;
	myOption.K = 65.0;
	myOption.T = 0.25;
	myOption.r = 0.08;
	myOption.sig = 0.3;
	myOption.type = 1;	// Put -1, Call +1
	double S_0 = 60;
	
	long N = 100;
	cout << "Number of subintervals in time: ";
	cin >> N;

	// Create the basic SDE (Context class)
	Range<double> range (0.0, myOption.T);
	double VOld = S_0;
	double VNew;

	ublas::vector<double> x = range.mesh(N);
	

	// V2 mediator stuff
	long NSim = 50000;
	cout << "Number of simulations: ";
	cin >> NSim;

	double k = myOption.T / double (N);
	double sqrk = sqrt(k);

	// Normal random number
	double dW;
	double price = 0.0;	// Option price
	double finalPrice;
	double sum_CT = 0.0; double sum_CT2 = 0.0;
	double SD, SE;

	boost::mt19937 rng;
	rng.seed(static_cast<boost::uint32_t> (std::time(0)));
	boost::normal_distribution<> nor = boost::normal_distribution<>(0.0,1.0);
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > myNormal 
					= boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > (rng, nor);

	using namespace SDEDefinition; 
	data = &myOption;

	vector<double> res;

	boost::timer t;
	// A.
	for (long i = 1; i <= NSim; ++i)
	{ // Calculate a path at each iteration
			
		if ((i/10000) * 10000 == i)
		{// Give status after each 10000th iteration

				cout << i << endl;
		}

		VOld = S_0;
		for (long index = 1; index < x.size(); ++index)
		{

			// Create a random number
			dW = myNormal();
				
			// The FDM (in this case explicit Euler)
			VNew = VOld  + (k * drift(x[index-1], VOld))
						+ (sqrk * diffusion(x[index-1], VOld) * dW);

			VOld = VNew;

		}
			
		// Payoff prices at t = T for current simulation
		double tmp = myOption.myPayOffFunction(VNew);
		price += (tmp)/double(NSim);

		sum_CT += tmp;
		sum_CT2 += tmp*tmp;

		finalPrice = price;
	}
	
	// D. Finally, discounting the average price
	finalPrice *= exp(-myOption.r * myOption.T);

	SD = sqrt((sum_CT2 - sum_CT*sum_CT/NSim)) * exp(-2.0*myOption.r*myOption.T) / (NSim - 1.0);
	SE = SD/sqrt(double(NSim));

	cout << "Price, after discounting: " << finalPrice << ", " << endl;
	cout << " StdDeviation, StdError " << SD << ", " << SE << endl;
	cout<<"Time for MC simulation: " << t.elapsed() << endl;

	return 0;
}