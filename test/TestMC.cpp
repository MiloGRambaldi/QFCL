/* qfcl/test/TestMC.cpp
 *
 * (C) Datasim Education BV 2006-2012
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

/*! \file test/TestMC.cpp
	\brief Some FD schemes for MC simulation and 1-factor option pricing.

	The design is based on Duffy's Domain Architectures (in this case RAT). Module
	interface by means of Boot signals and function.

	Main subsysystems:

		MC engine (main())
	    Payoff      
		FDM module (with fd schemes, SDE and RNG)
		Display
		MIS Agent
 
	For n-factor models we need to define the signature of signals and slots (mostly data) by making it more 
	generic.

	\author Datasim Education BV, James Hirschorn
	\date September 2, 2012
*/

#define QFCL_TEST_MC_VERSION 1.3
#define QFCL_NUM_SIMULATIONS 10000
#define QFCL_NUM_STEPS 200
#define QFCL_PROGRESS_INTERVAL 1000
#define QFCL_PRECISION 7
#define QFCL_NUM_BINS 60
#define	QFCL_NUM_ROWS 50

#define	QFCL_ENGINE MT19937
#define QFCL_FDM ExplicitEuler

// warning caused by boost::ublas
#pragma warning(disable:4996)

#include <cmath>
#include <string>

using namespace std;

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/placeholders.hpp>
namespace mpl = boost::mpl;
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <boost/type_traits/is_same.hpp>

#include <qfcl/defines.hpp>
#include <qfcl/random/engine/counting.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/random/engine/numberline.hpp>
#include <qfcl/utility/type_selection.hpp>

#include <qfcl/mc1/FDMVisitor_named.hpp>
#include <qfcl/mc1/FDMVisitor.cpp>
#include <qfcl/mc1/MCMediator.hpp>
#include <qfcl/mc1/SdeOneFactor.hpp>
using namespace qfcl::mc1;

#include "engine_common.ipp"

void ProgressPrint(long index, size_t interval)
{ // 'Subsystem' that implements progress and run-time statistics
    if (index == 0)
    {
        std::cout << "\nSimulations completed: ";
    }

    if (( (index + 1) / interval ) * interval == index + 1)
    {
        if (index + 1 != interval)
            cout << ", ";
        std::cout << index + 1;
    }
}

// list of FDM schemes
//struct fdm_schemes
//{
//	typedef mpl::vector< ExplicitEuler_named<double, double, double, mpl::_1>,
//						 ExplicitEulerTypeII_named<double, double, double, mpl::_1>,
//						 ExplicitEulerMM_named<double, double, double, mpl::_1>,
//						 RichardsonEuler_named<double, double, double, mpl::_1>,
//						 PredictorCorrector_named<double, double, double, mpl::_1>,
//						 PredictorCorrectorClassico_named<double, double, double, mpl::_1>,
//						 Milstein_named<double, double, double, mpl::_1>,
//						 KarhunenLoeve_named<double, double, double, mpl::_1>,
//						 PredictorCorrectorKL_named<double, double, double, mpl::_1>
//					   > list;
//	typedef mpl::bool_<true> named;
//	typedef mpl::bool_<true> is_PlaceholderExpression;
//};
typedef mpl::vector< 
	ExplicitEuler_named<double, double, double, mpl::_1>
> fdm_schemes;
//typedef mpl::vector< 
//	ExplicitEuler_named<double, double, double, mpl::_1>,
//	ExplicitEulerTypeII_named<double, double, double, mpl::_1>,
//	ExplicitEulerMM_named<double, double, double, mpl::_1>,
//	RichardsonEuler_named<double, double, double, mpl::_1>,
//	PredictorCorrector_named<double, double, double, mpl::_1>,
//	PredictorCorrectorClassico_named<double, double, double, mpl::_1>,
//	Milstein_named<double, double, double, mpl::_1>,
//	KarhunenLoeve_named<double, double, double, mpl::_1>,
//	PredictorCorrectorKL_named<double, double, double, mpl::_1>
//> fdm_schemes;

template<typename CounterType>
struct MC_functor
{
	MC_functor(CounterType num_sim, CounterType steps, 
		       bool disp, bool hist_disp, size_t progress_interval_, 
			   size_t prec, size_t nbins, size_t nrows) 
		: NSimulations(num_sim), N(steps), 
		  progress_display(disp), histogram_display(hist_disp), progress_interval(progress_interval_),
		  precision(prec), num_bins(nbins), num_rows(nrows)
	{
		// ACTIVATE THE MDODEL OF CHOICE HERE!

		using namespace OneFactorSDE;
		//using namespace CIRSDE;

		// Create the basic SDE (Context class)
		Range<double> range (0.0, T);
		sde = Sde<double, double, double>(initialCondition, range, drift, driftCorrected, diffusion, diffusionDerivative);
	}

	typedef void result_type;

	template<typename Engine, typename FDM>
	result_type operator()(Engine & eng, FDM & fdm);

	const CounterType NSimulations;
	const CounterType N;
	Sde<double,double,double> sde;

	const bool progress_display;
	const bool histogram_display;
	const size_t progress_interval;
	const size_t precision;
	const size_t num_bins;
	const size_t num_rows;
};

template<typename CounterType>
template<typename Engine, typename FDM>
void MC_functor<CounterType>::operator()(Engine & e, FDM & f)
{
	// ACTIVATE THE MDODEL OF CHOICE HERE!

	using namespace OneFactorSDE;
	// Management module for settings etc.
	MCMisAgent<Engine, FDM, CounterType> misAgent(NSimulations, N);
	
	/// NOTE: using command line instead
	// Settings and Factories from MIS
	//long NSimulations = 10000; long N = 200; bool progressDisplay;
	//misAgent(NSimulations, N, progressDisplay);
	

	// Factories (compile-time because we are using templates)
/*	boost::lagged_fibonacci607 rng;
	ExplicitEuler<double,double,double,boost::lagged_fibonacci607> fdm(N, sde, rng); // N == number of intervals
	MCTypeDMediator<boost::lagged_fibonacci607> mediator(fdm, NSimulations, myPayOffFunction, ProgressPrint, progressDisplay);

*/
	Engine eng;

	eng.seed(static_cast<boost::uint32_t> (std::time(0)));

	// The schemes; N == number of intervals
	//..

	FDM fdm(N, sde, eng);

	//ExplicitEuler<double,double,double,Engine> fdm(N, sde, eng); // Good ole one

	//ExplicitEulerTypeII<double,double,double,Engine> fdm(N, sde, rng); // Precompute rn array
	//ExplicitEulerMM<double,double,double,Engine> fdm(N, sde, rng); // MM == Method of moments
	//RichardsonEuler<double,double,double,Engine> fdm(N, sde, rng); // Higher order accuracy
	//PredictorCorrector<double,double,double,Engine> fdm(N, sde, rng, 0.5, 0.5); // PC with adjusted drift
	//PredictorCorrectorClassico<double,double,double,Engine> fdm(N, sde, rng, 0.5, 0.5); // No drift adjustmen
	//Milstein<double,double,double,Engine> fdm(N, sde, rng); // Slight improvement on Euler

	// Karhunen-Loeve family
	double tol = 0.01; // Determines number of terms in truncated KL expansion
	//cout << "tol: "; cin >> tol;
	//KarhunenLoeve<double,double,double,Engine> fdm(N, sde, rng, tol); // Euler + KL
	//KarhunenLoeve<double,double,double,boost::lagged_fibonacci607> fdm(N, sde, rng, tol); // Euler + KL
	//PredictorCorrectorKL<double,double,double,Engine> fdm(N, sde, rng, 0.5, 0.5, tol); // PC + KL
				
	MCReporter mcr(precision, histogram_display, num_bins, num_rows);

	MCTypeDMediator<double, long, Engine, double (*)(pathType<double>), MCReporter> //double), MCReporter> 
		mediator(fdm, mcr, NSimulations, myPayOffFunction, 
                 boost::bind(ProgressPrint, _1, progress_interval), progress_display);
			
	boost::signal<void (Status)> slotControl;
		
	try
	{
		// Connect signals to slots. N.B. use Boost references, otherwise a copy is
		// made and you will get incorrect results.
		slotControl.connect(boost::ref(misAgent)); // Create a reference to mcr
		
		slotControl(START);						// Signal to start process, timer

		mediator.price();						// GET THE PRICE, SD, SE

		slotControl(STOP);						// Signal to stop receiving data

	}
	catch(std::exception& exception)
	{ 
		cout << exception.what() << endl;
		exit(1);
	}
}

template<typename CounterType>
MC_functor<CounterType> MC_creator(CounterType NSimulations, CounterType N,
								   bool progress_display, bool histogram_display, size_t progress_interval, 
								   size_t precision, size_t num_bins, size_t num_rows)
{
	return MC_functor<CounterType>(NSimulations, N, progress_display, histogram_display, progress_interval, 
								   precision, num_bins, num_rows);
}

template<typename T>
bool list_parameter_options(const string & option, const string & param)
{
	auto names = qfcl::names::get_names<T>();
	
	if (param == "help" || param == "h")
	{
		ostringstream oss;
		const size_t indent_width = 5;

		BOOST_FOREACH(string s, names)
		{
			oss << setw(indent_width) << "" << s << endl;
		}

		cout << oss.str();

		return true;
	}
	else
	{
		if (find(begin(names), end(names), param) == end(names))
			throw po::invalid_option_value(param + " is not a valid option.");
	}

	return false;
}

template<typename T1, typename T2>
bool list_parameter_options(const string & option, const string & param)
{
    typedef typename mpl::if_<
            typename T2::is_PlaceholderExpression,
            typename qfcl::tmp::InstantiateList< T2, typename mpl::deref< typename mpl::begin<T1>::type >::type >,
            T2
    >::type Derived;

	return list_parameter_options<Derived>(option, param);
}

int main(int argc, char * argv[])
{
	using namespace qfcl::tmp;
	using namespace qfcl::type_selection;

	typedef size_t CounterType;

	CounterType NSimulations; 
	CounterType N; 

	size_t prec;
	size_t progress_interval;
	size_t num_bins;
	size_t num_rows;
	string engine_param;
	string fdm_param;

	// Declare the supported options.
	po::options_description generic_options("Generic options");
	generic_options.add_options()
		("help,h", "this help message")
		("version,v", "version info");

	po::options_description engine_options("Engine options");
	engine_options.add_options()
		("engine,e", 
		 po::value<string>(&engine_param) -> default_value(stringize(QFCL_ENGINE)),
		 "engine to use. Type -e h [ --engine help ] for a list of all available engines.");

	po::options_description sde_options("SDE options");
	sde_options.add_options()
		("FDM_Scheme,f",
		 po::value<string>(&fdm_param) -> default_value(stringize(QFCL_FDM)),
		 "FDM scheme to use for simulating the SDE. Type -f h [ --FDM_Scheme help ] for a list of all available FDM schemes.");

	po::options_description primary_options("Alternatives to positional command line parameters");
	primary_options.add_options()
		("simulations,s", po::value<CounterType>(&NSimulations) -> default_value(QFCL_NUM_SIMULATIONS), 
		 "number of simulations")
		("steps,N", po::value<CounterType>(&N) -> default_value(QFCL_NUM_STEPS),
		 "number of steps for finite difference scheme"); 

	po::options_description output_options("Output options");
	output_options.add_options()
		("no_progress,n", "suppress progress display")
		("progress_interval,i", po::value<size_t>(&progress_interval) -> default_value(QFCL_PROGRESS_INTERVAL),
		 "display progress every i-th simulation (unless --no_progress is set)")
		("precision,p", po::value<size_t>(&prec) -> default_value(QFCL_PRECISION),
		 "output precision")
		("histogram,g", "display a histogram of simulated results")
		("bins,b", po::value<size_t>(&num_bins) -> default_value(QFCL_NUM_BINS),
		 "number of bins in the histogram(s)")
		("rows,r", po::value<size_t>(&num_rows) -> default_value(QFCL_NUM_ROWS),
		 "height of the histogram(s) in number of rows");
	
	po::options_description command_line_options;
	command_line_options.add(generic_options).add(engine_options).add(sde_options).add(primary_options).add(output_options);

	po::positional_options_description pd;
	pd.add("simulations", 1);
	pd.add("steps", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(command_line_options).positional(pd).run(), vm);
	po::notify(vm);    

	// parse command line parameters

	if (vm.count("version"))
	{
		cout << argv[0] << ", Version " << QFCL_TEST_MC_VERSION << endl;
		cout << "(C) Datasim Education BV 2006-2012" << endl
			 << "Copyright 2012, James Hirschorn <James.Hirschorn@gmail.com>" << endl;
		return EXIT_SUCCESS;
	}

	// make sure that user is aware that there are options
	if (argc == 1 || vm.count("help")) 
	{
		cout << endl << endl;
		cout << argv[0] << " tests the MC1 Monte Carlo framework." << endl << endl;
		cout << "Usage: " << argv[0] << " [options] number_of_simulations number_of_steps_for_FDM" << endl
			 << "   OR  " << argv[0] << " [options]" << endl << endl;
		cout << "Example: " << argv[0] << " 100000 500 -i 10000 -e boost-MT19937 -f Milstein -g" << endl << endl;
		cout << generic_options << endl;
		cout << engine_options << endl;
		cout << sde_options << endl;
		cout << primary_options << endl;
		cout << output_options << endl;
		if (vm.count("help"))
			return EXIT_SUCCESS;
	}
	else
		cout << endl;

	bool progressDisplay = !vm.count("no_progress");
	bool histogramDisplay = vm.count("histogram") != 0;
	
	if (list_parameter_options<all_engines>("engine", engine_param))
		return EXIT_SUCCESS;

	typedef mpl::transform< fdm_schemes, mpl::apply1<mpl::_1, mpl::deref< mpl::begin<all_engines>::type >::type> >::type Derived;
//	typedef qfcl::tmp::InstantiateList< fdm_schemes, mpl::deref< mpl::begin<all_engines>::type >::type > Derived;
	if (list_parameter_options<Derived>("FDM_Scheme", fdm_param))
		return EXIT_SUCCESS;

	auto mc = MC_creator(NSimulations, N, progressDisplay, histogramDisplay, progress_interval, 
						 prec, num_bins, num_rows);
	
	typedef mpl::vector< qfcl::random::mt19937 > some_engines;
	for_each_selector<some_engines, fdm_schemes, IDENTITY, INSTANTIATION>(engine_param, fdm_param, mc); 

}
