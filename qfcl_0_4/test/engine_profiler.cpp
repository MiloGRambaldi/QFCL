/* test/engine_profiler.cpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt)
 */

/*! \file test/engine_profiler.cpp
	\brief engine profiler

	\author James Hirschorn
	\date August 7, 2012
*/

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
using std::string;

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/ref.hpp>
#include <boost/chrono.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/bind/placeholders.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <boost/random.hpp>
#include <boost/ref.hpp>
#include <boost/timer/timer.hpp>
#include <boost/type_traits/is_same.hpp>

#include <qfcl/defines.hpp>
#include <qfcl/utility/comma_separated_number.hpp>
#include <qfcl/random/engine/counting.hpp>
#include <qfcl/random/engine/named_adapter.hpp>
#include <qfcl/random/engine/numberline.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/random/engine/twisted_generalized_feedback_shift_register.hpp>
#include <qfcl/statistics/descriptive.hpp>
#include <qfcl/utility/type_selection.hpp>

#include "utility/cpu_timer.hpp"

#include "engine_common.ipp"

#define	QFCL_TIMING_PERCENTILE	0.9997
#define QFCL_NUM_BINS 30
#define	QFCL_NUM_ROWS 20
#define QFCL_PRECISION 7

#define	QFCL_ENGINE MT19937
#ifdef QFCL_RDTSCP
#define QFCL_TIMER RDTSCP
#else
#define QFCL_TIMER RDTSC
#endif

//#define SYSTEM_CPU_FREQUENCY (3.114E9)

template<typename Engine, typename CounterType>
boost::timer::cpu_times time_engine(Engine & e, CounterType iterations) 
{
	using namespace boost::timer;

	// Important! Prevent excessive optimization
	volatile typename Engine::result_type value;

	cpu_timer timer;

	// Note: For some reason this loop does not register as either user or system usage.
	for (; iterations > 0; --iterations)
	{
		value = e();
	}

	timer.stop();

	return timer.elapsed();
}

// The functor F must have a result_type member
template<typename F, typename TimerType, typename It, typename CounterType>
void time_routine(F & routine, TimerType & timer, It sample, It sample_end, CounterType inner_loop_size)
{
	volatile typename F::result_type value;

	for (; sample != sample_end; ++sample)
	{
		uint64_t start_loop = timer();

		for (CounterType count = 0; count < inner_loop_size; ++count)
			value = routine();

		uint64_t stop_loop = timer();

        *sample = static_cast<typename It::value_type>(stop_loop - start_loop) / inner_loop_size;
	}
}

template<typename CounterType>
void show_timing_results(boost::timer::cpu_times result, CounterType iterations, const string & engine_name, double cpu_freq)
{
	using namespace boost::timer;

	nanosecond_type time_taken = result.user + result.system;
	if (time_taken == 0)
		time_taken = result.wall;
	std::cerr << "Debug: wall = " << result.wall << ", user = " << result.user << ", system = " << result.system << std::endl;
    double time_taken_seconds = double(time_taken) / qfcl::timer::one_second;

	std::cout 
		<< boost::format("%|1$25|: %|2$.2f| random numbers/second = %|3$.8| nanoseconds/random number = %|4$5.1f| CPU cycles/random number\n\n")
			% engine_name % ( double(iterations) / time_taken_seconds ) 
			% ( double(time_taken) / iterations ) % (cpu_freq * time_taken_seconds / iterations);
}

template<typename CounterType> 
void show_timing_results(uint64_t clock_cycles, CounterType iterations, const string & engine_name, double cpu_freq)
{
	using namespace std;

	// time taken in seconds
	double time_taken = clock_cycles / cpu_freq;

	//std::cerr << "Debug: " << time_taken << "s" << std::endl;

	cout 
		<< boost::format("%|1$25|: %|2$.2f| random numbers/second = %|3$.8| nanoseconds/random number = %|4$5.1f| CPU cycles/random number\n")
			% engine_name % ( iterations / time_taken ) 
			% ( time_taken * UINT64_C(1000000000) / iterations ) 
			% ( double(clock_cycles) / iterations );
}

typedef double sample_t;

template<typename SamplePopulation>
void show_statistics(const SamplePopulation & population)
{
	using namespace std;
    using namespace boost::accumulators;

   // boost::timer::auto_cpu_timer timer;

    accumulator_set<
        sample_t,
        stats<
            tag::mean,
            tag::variance,
            tag::median
        >
    > acc;

    // accumulate statistics
    auto v = population.sample();
    for_each(QFCL_NONMEMBER_BEGIN(v), QFCL_NONMEMBER_END(v), boost::bind<void>(boost::ref(acc), _1));
    qfcl::statistics::DescriptiveStatistics<> qfcl_stats(population);

    cout << "Mean: " << mean(acc) << endl;
 // cout << "Mean: " << qfcl_stats.mean() << endl;
    cout << "Standard Error: " << sqrt(variance(acc)) << endl;
 // cout << "Standard Error: " << qfcl_stats.sd() << endl;
    cout << "Median: " << median(acc) << endl;
 // cout << "Median: " << qfcl_stats.median() << endl;
    cout << "Mode: ";
    auto mode = qfcl_stats.mode();
    copy( QFCL_NONMEMBER_BEGIN(mode), QFCL_NONMEMBER_END(mode), ostream_iterator<double>(cout) );
	cout << endl;
}

template<typename Engine, typename It, typename Timer>
void timing(Engine & e, It sample, It sample_end, double cpu_freq, Timer t)
{
	using namespace boost::timer;

	auto result = t(e, sample, sample_end, 0);

    show_timing_results(result, std::distance(sample, sample_end), boost::mpl::c_str<typename Engine::name>::value, cpu_freq);

	std::cout << std::endl;
}

// list of timers
//struct timers
//{
typedef mpl::vector< qfcl::timer::rdtsc_timer
#ifdef  QFCL_RDTSCP
                   , qfcl::timer::rdtscp_timer
#endif  // QFCL_RDTSCP
>::type timers;
//	typedef mpl::bool_<true> named;
//	typedef mpl::bool_<false> is_PlaceholderExpression;
//};

typedef unsigned long CounterType;

// This is where run-time type selection is performed
// (recall that there are no common base classes).
// It is implemented as a doubly nested compile-time for_each loop,
// where the outer loop is for the TimerType and the inner loop
// is for the EngineType.
template<typename EngineList, typename TimerList, typename It>
struct profiler
{
	profiler(string engine_param, string timer_param, It begin_samples_, It end_samples_, size_t loops_per_sample_) 
		: p(engine_param, timer_param, begin_samples_, end_samples_, loops_per_sample_) {}

	typedef void result_type;

	template<typename T1, typename T2>
	result_type operator()(T1 & t1, T2 & t2);

	//! uses trivial factory
	//template<typename T1>
	//T1 factory(T1 & t1)
	//{
	//	return T1();
	//}

	//template<typename T1, typename T2>
	//T2 factory(T1 & t1, T2 & t2)
	//{
	//	return T2();
	//}
private:
	struct parameters
	{
		parameters(string engine_param, string timer_param, It begin_samples_, It end_samples_, size_t loops_per_sample_) 
			: //engine_name(engine_param), timer_name(timer_param), 
			  begin_samples(begin_samples_), end_samples(end_samples_), 
			  loops_per_sample(loops_per_sample_) {}

	//	const string engine_name;
		const It begin_samples;
		const It end_samples;
		const size_t loops_per_sample;
	//	const string timer_name;
	} p;
};

template<typename EngineList, typename TimerList, typename It>
template<typename T1, typename T2>
void profiler<EngineList, TimerList, It>::operator()(T1 & t1, T2 & t2)
{
	T1 engine;
	T2 timer;

	time_routine( engine, timer, p.begin_samples, p.end_samples, p.loops_per_sample );
	// this is how run-time type selection is implemented
	//if (mpl::c_str<T::name>::value != p.timer_name)
	//	return;

	//mpl::for_each<EngineList>(engine_loop<T>(p));
}
//
//template<typename EngineList, typename TimerList, typename It>
//template<typename TimerType>
//template<typename T>
//void profiler<EngineList, TimerList, It>::engine_loop<TimerType>::operator()(T)
//{
//	// this is how run-time type selection is implemented
//	if (mpl::c_str<T::name>::value != p.engine_name)
//		return;
//
//	time_routine( T(), TimerType(), p.begin_samples, p.end_samples, p.loops_per_sample );
//}

template<typename EngineList, typename TimerList, typename It>
profiler<EngineList, TimerList, It> create_profiler(string engine_param, string timer_param, It begin_samples, It end_samples, size_t loops_per_sample)
{
	return profiler<EngineList, TimerList, It>(engine_param, timer_param, begin_samples, end_samples, loops_per_sample);
}

int main(int argc, char * argv[])
{
	using namespace std;
	using namespace qfcl::random;
	using namespace qfcl::statistics;
	using namespace qfcl::type_selection;

	size_t sample_size;
	size_t loops_per_sample;
	double excluded_quantile;

	size_t num_bins;
	size_t num_rows;
	size_t prec;

	string engine_param;
	string timer_param;

	// Declare the supported options.
	po::options_description generic_options("Generic options");
	generic_options.add_options()
		("help,h", "this help message")
		("version,v", "version info");

	po::options_description engine_option("Engine and timer options");
	engine_option.add_options()
		("engine,e", 
		 po::value<string>(&engine_param) -> default_value(stringize(QFCL_ENGINE)),
		 "engine to be profiled. Type -e h [ --engine help ] for a list of all available engines.")
		("timer,t", po::value<string>(&timer_param) -> default_value(stringize(QFCL_TIMER)),
		 "the time stamp counter read operation to use:\nRDTSC  - not serializing\nRDTSCP - serializing.");

	po::options_description output_options("Output options");
	output_options.add_options()
		("bins,b", po::value<size_t>(&num_bins) -> default_value(QFCL_NUM_BINS),
		 "number of bins in the histogram(s)")
		("rows,r", po::value<size_t>(&num_rows) -> default_value(QFCL_NUM_ROWS),
		 "height of the histogram(s) in number of rows")
		("no_freq,n", "suppress display of frequencies of each time measurement")
		("precision,p", po::value<size_t>(&prec) -> default_value(QFCL_PRECISION),
		 "output precision for histograms");

	po::options_description primary_options("Alternatives to positional command line parameters");
	const string QFCL_TIMING_PERCENTILE_DESC = stringize(QFCL_TIMING_PERCENTILE);
	primary_options.add_options()
		("sample_size,s", po::value<size_t>(&sample_size) -> default_value(QFCL_PROFILER_SAMPLESIZE), 
		 "set the sample size")
		("loops_per_sample,l", po::value<size_t>(&loops_per_sample) -> default_value(1),
		 "set the number of loops in each sample")
		("quantile,q", po::value<double>(&excluded_quantile) -> default_value(QFCL_TIMING_PERCENTILE, QFCL_TIMING_PERCENTILE_DESC),
		 "exclude this quantile of timings (between 0 and 1)");

	po::options_description command_line_options;
	command_line_options.add(generic_options).add(engine_option).add(output_options).add(primary_options);

	po::positional_options_description pd;
	pd.add("sample_size", 1);
	pd.add("loops_per_sample", 1);
	pd.add("quantile", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(command_line_options).positional(pd).run(), vm);
	po::notify(vm);    

	if (vm.count("version"))
	{
		cout << argv[0] << ", Version " << QFCL_VERSION << endl;
		cout << "Copyright 2012, James Hirschorn <James.Hirschorn@gmail.com>" << endl;
		return EXIT_SUCCESS;
	}

	// make sure that user is aware that there are options
	if (argc == 1 || vm.count("help")) 
	{
		cout << argv[0] << " measures the time taken to generate individual\n" \
				"pseudo-random numbers for various engines. Units are clock cycles." << endl << endl;
		cout << "Usage: " << argv[0] << " [options] sample_size [loops_per_sample] [quantile]" << endl
			 << "   OR  " << argv[0] << " [options]" << endl << endl;
		cout << generic_options << endl;
		cout << engine_option << endl;
		cout << primary_options << endl;
		cout << output_options << endl;
		if (vm.count("help"))
			return EXIT_SUCCESS;
	}

	// list of engines
	if (vm.count("engine"))
	{
		auto engine_names = qfcl::names::get_names<all_engines>();

		if (engine_param == "help" || engine_param == "h")
		{
			using std::string;
			std::ostringstream oss;
			const size_t indent_width = 5;

			BOOST_FOREACH(std::string s, engine_names)
			{
				oss << std::setw(indent_width) << "" << s << std::endl;
			}

			cout << oss.str();

			return EXIT_SUCCESS;
		}
		else
		{
            if (find(QFCL_NONMEMBER_BEGIN(engine_names), QFCL_NONMEMBER_END(engine_names), engine_param) == QFCL_NONMEMBER_END(engine_names))
				throw po::invalid_option_value(engine_param + " is not a valid engine.");
		}
	}

	cout << "Sample size: " << qfcl::io::custom_formatted(sample_size) << endl;
	cout << "Loops per sample: " << loops_per_sample << endl;
	cout << "Excluded quantile: " << excluded_quantile << endl;

#ifndef	SYSTEM_CPU_FREQUENCY
    double cpu_frequency = qfcl::timer::get_cpu_frequency();
#else
	double cpu_frequency = SYSTEM_CPU_FREQUENCY;
#endif	// SYSTEM_CPU_FREQUENCY

    cout << boost::format("CPU frequency: %|1$.5| GHz\n\n") % (cpu_frequency / qfcl::timer::one_second);

	cout << "Engine: " << engine_param << endl << endl;

	vector<sample_t> samples(sample_size);
	
    auto profiler = create_profiler<all_engines, timers>(
                engine_param,
                timer_param,
                QFCL_NONMEMBER_BEGIN(samples),
                QFCL_NONMEMBER_END(samples),
                loops_per_sample
    );

	cout << "Start timing using " << timer_param << " ...";

	// uses run-type type selection so that only the chosen engine is used 
	for_each_selector<all_engines, timers, NAME_OR_TYPENAME, NAME>(engine_param, timer_param, profiler);

	cout << " timing completed." << endl << endl;
	
    DescriptiveStatistics<> all_stats( QFCL_NONMEMBER_BEGIN(samples), QFCL_NONMEMBER_END(samples) );

	DescriptiveStatistics<> stats = all_stats.Tail(excluded_quantile);
	cout << qfcl::io::custom_formatted(stats.size()) << " samples after excluding quantile.\n\n";
	if (!vm.count("no_freq"))
    {
		cout << "Frequencies of occurrences:" << endl;
		stats.ShowFrequencies(cout);
	}
	cout << "Distribution of time measurements (log scale):" << endl;
	stats.log_distribution_histogram(cout, num_bins, -numeric_limits<sample_t>::infinity(), numeric_limits<sample_t>::infinity(), 
									 num_rows, prec);
	show_statistics(stats);

	cout << endl;

	cout << "Press Enter to exit.";

	char c;
	cin.get(c);
}
