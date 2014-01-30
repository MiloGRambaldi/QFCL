// MCPostProcess.hpp
//
// A Slot class that gets data from a MC signal 'emitter'. This corresponds to a MIS
// system that postprocesses data from the MC engine.
//
// (C) Datasim Education BV 2011
//

#ifndef MCPost_hpp
#define MCPost_hpp

#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <limits>
#include <list>
#include <vector>

#include <boost/mpl/string.hpp>
namespace mpl = boost::mpl;
#include <boost/numeric/ublas/vector.hpp>
#include <boost/signals.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/timer/timer.hpp>

#include <qfcl/statistics/descriptive.hpp>

#include <qfcl/utility/tmp.hpp>

#include "SdeOneFactor.hpp"
enum Status {START, STOP};


// Some statistics-based functions
template <typename V>
	boost::tuple<double, double> SumSquaresAndSum(const V& container)
{ // Sum and sum of squares of a container

	double sum = 0.0;
	double sumSquares = 0.0;

	std::for_each(container.begin(), container.end(), [&sum, &sumSquares] (double d) { sum += d; sumSquares += d*d;});

	return boost::tuple<double, double> (sum, sumSquares);
}


template <typename V>
	boost::tuple<double, double> StandardDeviationAndError(const V& container, double r, double T)
{ // Standard deviation and error

	double sum;;
	double sumSquares;
	double M = double(container.size());

	boost::tie(sum, sumSquares) = SumSquaresAndSum<V>(container);

	// Now compute SD and SE
	double SD = (sqrt(sumSquares - (sum*sum)/M) * exp(-2.0*r*T)) / (M - 1.0);
	SD = sqrt( (sumSquares - (sum*sum)/M) * exp(-2.0*r*T) / (M - 1.0) );
	double SE = SD/sqrt(M);

	return boost::tuple<double, double> (SD, SE);
}
	
template<typename Engine, typename FDM, typename CounterType>
struct MCMisAgent
{
	boost::timer::cpu_timer t;

	CounterType NSim, N;

	MCMisAgent(CounterType simulations, CounterType steps) : NSim(simulations), N(steps) {}
	//MCMisAgent() : t(boost::cpu_timer()) {}

	void operator () (Status s)
	{
		if (s == START)
		{
			cout << "\nMIS Configuration information for the MC Engine, starting up ...\n";
			cout << "Number of simulations: " << NSim << endl;
			cout << "Number of steps for FDM: " << N << endl;
            cout << "Pseudo-random number generator used: " << mpl::c_str<typename Engine::name>::value << endl;
            cout << "FDM scheme used: " << mpl::c_str<typename FDM::name>::value << endl;
		}
		else if (s == STOP)
		{
			cout << "\nMIS, MC simulation completed. "; //final results: ";
			cout << "Time taken for MC simulation:\n" << t.format() << endl;
		}
	}

	void operator () (long& NSIM, long& NT, bool& progressBar)
	{ // Simulation parameters

		cout << "NT: ";  cin >> NT;
		cout << "NSIM: "; cin >> NSIM;
		cout << "Progress Yes/N: "; char a; cin >> a;
		if (a == 'y' || a == 'Y')
		{
			progressBar = true;
		}
		else
		{
			progressBar = false;
		}

	}

};


struct MCReporter
{
	typedef boost::numeric::ublas::vector<double> arrayType;
	arrayType arr;

	const size_t prec;
	size_t num_bins;
	size_t num_rows;

	const bool histogram;
	const bool histogram_default; // use default number of bins and rows
	static const size_t num_bins_default = 30;

	MCReporter(size_t output_precision, bool show_histogram, size_t nbins, size_t nrows) 
		: prec(output_precision), num_bins(nbins), num_rows(nrows), 
		  histogram(show_histogram), histogram_default(false)// why? : arr(boost::numeric::ublas::vector<double>())
	{
		
	}

	MCReporter(size_t output_precision, bool show_histogram) 
		: prec(output_precision), 
		  histogram(show_histogram), histogram_default(true)// why? : arr(boost::numeric::ublas::vector<double>())
	{
		
	}


	void operator () (Status s)
	{
		if (s == START)
		{
//			std::cout << "Start " << arr.size() << std::endl;
		}
		else if (s == STOP)
		{
			using namespace std;
			using namespace qfcl::statistics;
			// Print using lambda functions
			using namespace OneFactorSDE;
		
			/*
			// Calculate the price
			//	double initVal = 0.0;
			//	double sum = std::accumulate(arr.begin(), arr.end(), initVal);
			
			// Sum of option values at terminal time T
			double sum = 0.0;
			std::for_each(arr.begin(), arr.end(), [&sum] (double d) { sum += d; });

			double price = exp(-r*T) * sum / double (arr.size());
			std::cout << "\nPrice: " << price << ". ";

			boost::tuple<double, double> answer = StandardDeviationAndError<boost::numeric::ublas::vector<double>> (arr, r, T);
			std::cout << " StdDeviation, StdError " << answer << std::endl;

			*/

			cout << endl << endl;

			// discount the terminal values

			arrayType discounted_arr = arr;
			auto discount_factor = exp(-r * T);
			for_each(begin(discounted_arr), end(discounted_arr), [=] (double & d) {d *= discount_factor;});
			DescriptiveStatistics<> stats(begin(discounted_arr), end(discounted_arr));
			
			// save the stream state
			auto store_flags = cout.flags();
			auto store_prec = cout.precision();

			cout.setf(std::ios::fixed);
			cout.precision(prec);

			if (histogram)
			{
				cout << "Distribution of simulated prices:" << endl;
				if (histogram_default)
					stats.distribution_histogram(cout, num_bins_default, -numeric_limits<double>::infinity(), numeric_limits<double>::infinity());
				else
					stats.distribution_histogram(cout, num_bins, -numeric_limits<double>::infinity(), numeric_limits<double>::infinity(),
												 num_rows);
			}

			cout << "Price: " << stats.mean() << endl;
			cout << "Standard deviation: " << stats.sd() << endl;
			cout << "Standard error: " << stats.se() << endl;
			cout << "Median price: " << stats.median() << endl;
			cout << "Fisher skew: " << stats.skew() << endl;
			cout << "Excess kurtosis: " << stats.ExcessKurtosis() << endl;

			// restore the stream state
			cout.flags(store_flags);
			cout.precision(store_prec);
		}
	}

	void operator () (const boost::numeric::ublas::vector<double>& packetArr)
	{
		arr = packetArr;
	}
};


#endif
