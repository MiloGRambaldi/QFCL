/* test/engine_speed.cpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt)
 */

/*! \file test/engine_speed.cpp
	\brief engine speed test

	\author James Hirschorn
	\date March 22, 2012
*/

#include <iostream>
#include <stdexcept>
#include <string>
using std::string;
#include <vector>
using std::vector;

#include <boost/chrono.hpp>
#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/mpl/push_front.hpp>
namespace mpl = boost::mpl;
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <boost/random.hpp>
#include <boost/timer/timer.hpp>

#include <qfcl/defines.hpp>
#include <qfcl/random/engine/counting.hpp>
#include <qfcl/random/engine/named_adapter.hpp>
#include <qfcl/random/engine/numberline.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/random/engine/twisted_generalized_feedback_shift_register.hpp>
#include <qfcl/utility/comma_separated_number.hpp>
#include <qfcl/utility/names.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/type_selection.hpp>

#include "utility/cpu_timer.hpp"

#include "engine_common.ipp"

// default timer
#define QFCL_TIMER rdtsc_macro
// default number of iterations per engine
#define	QFCL_ITERATIONS 100000000

// If you want to include speed tests for engines not satisfying the Named concept
// (e.g. the boost::random engines)
//#define INCLUDE_UNNAMED

/** timers */

/* timing routines */

// boost timer
template<typename Engine, typename CounterType>
inline
boost::timer::cpu_times time_engine_boost(Engine & e, CounterType iterations) 
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

#ifdef QFCL_RDTSCP
typedef qfcl::timer::rdtscp_timer timer_type;
#else
typedef qfcl::timer::rdtsc_timer timer_type;
#endif

//time stamp counter
template<typename Engine, typename CounterType>
inline
uint64_t time_engine_TSC(Engine & e, CounterType iterations) 
{
	volatile typename Engine::result_type value;

    timer_type timer;

	uint64_t start = timer();

	for (; iterations > 0; --iterations)
	{
		value = e();
	}

	uint64_t end = timer();

	return end - start;
}

#define TIMER_MACRO_TSC(Engine, e, iter, result)\
	volatile typename Engine::result_type value;\
												\
    timer_type timer;							\
												\
	uint64_t start = timer();					\
												\
	for (CounterType i = 0; i < iter; ++i)		\
	{											\
		value = e();							\
	}											\
												\
	uint64_t end = timer();						\
												\
	result = end - start


// output results

template<typename CounterType>
void show_timing_results(uint64_t clock_cycles, CounterType iterations, const string & engine_name, double cpu_freq)
{
	// time taken in seconds
	double time_taken = clock_cycles / cpu_freq;

	std::cout 
		<< boost::format("%|1$25|: %|2$13.2f| random numbers/second = %|3$11.8f| nanoseconds/random number = %|4$5.1f| CPU cycles/random number\n\n")
			% engine_name % ( iterations / time_taken ) 
			% ( time_taken * UINT64_C(1000000000) / iterations ) 
			% ( double(clock_cycles) / iterations);
}

template<typename CounterType>
void show_timing_results(boost::timer::cpu_times t, CounterType iterations, const string & engine_name, double cpu_freq)
{
	uint64_t clock_cycles = static_cast<uint64_t>( (t.user + t.system) * cpu_freq / UINT64_C(1000000000) );

	show_timing_results(clock_cycles, iterations, engine_name, cpu_freq);
}

/* timer object */

// generic
template<typename CounterType, typename Timer>
struct timer_object
{
	timer_object(CounterType iter, double freq) 
		: iterations_(iter), cpu_freq_(freq) {} 

	template<typename Engine>
	void operator()(Engine & e) 
	{
		Timer t;
		auto result = t(e, iterations_);

#ifdef INCLUDE_UNNAMED
		show_timing_results(result, iterations_, qfcl::names::name_or_typename(e), cpu_freq_);
#else
		show_timing_results(result, iterations_, qfcl::names::name(e), cpu_freq_);
#endif // INCLUDE_UNNAMED
	}

	CounterType iterations_;
	double cpu_freq_;
};

// specific timers

namespace detail {
	typedef mpl::string<'r','d','t','s','c'>::type rdtsc_string;
	typedef mpl::string<'m','a','c','r','o'>::type macro_string;
	typedef qfcl::tmp::concatenate<rdtsc_string, mpl::string<'_'>::type, macro_string>::type rdtsc_macro_string;
	typedef mpl::string<'b','o','o','s','t'>::type boost_string;
}	// namespace detail

struct boost_timer
{
	typedef boost::timer::cpu_times result_type;

	template<typename Engine, typename CounterType>
	result_type operator()(Engine &, CounterType iterations) const
	{
		Engine e;
		return time_engine_boost(e, iterations); 
	}

	static string description()
	{
		return "boost::timer, engine passed by reference";
	}

	typedef detail::boost_string name;
};

struct rdtsc
{
	typedef uint64_t result_type;

	template<typename Engine, typename CounterType>
	result_type operator()(Engine &, CounterType iterations) const
	{
		Engine e;
		return time_engine_TSC(e, iterations); 
	}

	static string description()
	{
		return "time stamp counter (non-serialized), engine passed by reference";
	}

	typedef detail::rdtsc_string name;
};

struct rdtsc_macro
{
	typedef uint64_t result_type;

	template<typename Engine, typename CounterType>
	result_type operator()(Engine &, CounterType iterations) const
	{
		Engine e;
		result_type result;
		TIMER_MACRO_TSC(Engine, e, iterations, result);
		return result;
	}

	static string description()
	{
		return "time stamp counter (non-serialized) as a macro";
	}

	typedef detail::rdtsc_macro_string name;
};

// list of timers
typedef mpl::vector<rdtsc, rdtsc_macro, boost_timer> timer_list;

// We want to avoid double type-selection for now, so we use the following "kludge".
template<typename EngineList, typename CounterType, typename SelectionMethod>
void perform_speed_test(const vector<string> & engine_params, const string & timer_name, CounterType iterations, double cpu_freq)
{
	if (timer_name == "rdtsc")
        qfcl::type_selection::for_each_selector<EngineList, SelectionMethod>(
			engine_params,
			timer_object<CounterType, rdtsc>(iterations, cpu_freq)
		);
	else if (timer_name == "rdtsc_macro")
        qfcl::type_selection::for_each_selector<EngineList, SelectionMethod>(
			engine_params, 
			timer_object<CounterType, rdtsc_macro>(iterations, cpu_freq)
		);
	else if (timer_name == "boost")
        qfcl::type_selection::for_each_selector<EngineList, SelectionMethod>(
			engine_params, 
			timer_object<CounterType, boost_timer>(iterations, cpu_freq)
		);
	else
        throw std::logic_error("bad program");
}

/** output function object */

// boost::lambda did not work for this.
struct named_functor
{
	typedef void result_type;

	named_functor(std::ostringstream & oss, size_t indent_width)
		: oss_(oss), indent_width_(indent_width)
	{};

	template<typename T>
	result_type operator()(T & t)
	{
		oss_ << std::setw(indent_width_) << "" << qfcl::names::name(t) << ": " 
			<< T::description() << std::endl;
	}
	
private:
	std::ostringstream & oss_;
	size_t indent_width_;
};

int main(int argc, char * argv[])
{
	using namespace std;
	using namespace qfcl::random;
	using namespace qfcl::type_selection;

	typedef unsigned long long CounterType;
	//typedef unsigned long CounterType;

	CounterType iterations;

	// Declare the supported options.
	po::options_description generic_options("Generic options");
	generic_options.add_options()
		("help,h", "this help message")
		("version,v", "version info");
	
	std::string timer_param;

	po::options_description engine_option("Engine and Timer options");
	engine_option.add_options()
		("engine,e", 
		 po::value< vector<string> >() -> composing(),
		 "specifies an engine to test. Use this options multiple times to specify a list of engines. " \
		 "All engines are tested if this option is not specified. " \
		 "Type -e h [ --engine help ] for a list of all available engines.")
		("timer,t", po::value<string>(&timer_param) -> default_value(stringize(QFCL_TIMER)),
		 "specifies which timer to use. " \
		 "Type -t h [ --timer help ] for a list of all timers.");

	po::options_description primary_options("Alternatives to positional command line parameters");
	primary_options.add_options()
		("iterations,i", po::value<CounterType>(&iterations) -> default_value(QFCL_ITERATIONS),
		 "number of random numbers to generate for each engine");

	po::options_description command_line_options;
	command_line_options.add(generic_options).add(engine_option).add(primary_options);

	po::positional_options_description pd;
	pd.add("iterations", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(command_line_options).positional(pd).run(), vm);
	po::notify(vm);    

	if (vm.count("version"))
	{
		cout << argv[0] << ", QFCL Version " << QFCL_VERSION << endl;
		cout << "Copyright 2012, James Hirschorn <James.Hirschorn@gmail.com>" << endl;
		return EXIT_SUCCESS;
	}

	// make sure that user is aware that there are options
	if (argc == 1 || vm.count("help")) 
	{
		cout << argv[0] << " measures the speed of pseudo-random number generation\n" \
			 << "of various engines." << endl << endl;
		cout << "Usage: " << argv[0] << " [options] [iterations]" << endl
			 << "   OR  " << argv[0] << " [options]" << endl << endl;
		cout << "Example: " << argv[0] << " -e MT19937 -e boost-MT19937 10000000" << endl << endl;
		cout << generic_options << endl;
		cout << engine_option << endl;
		cout << primary_options << endl;
		if (vm.count("help"))
			return EXIT_SUCCESS;
	}

	vector<string> engine_params; 
#ifdef INCLUDE_UNNAMED
	typedef mpl::push_front<all_engines, boost::random::mt19937>::type engine_list;
#else
	typedef all_engines engine_list;
#endif // INCLUDE_UNNAMED

	auto engine_names = qfcl::names::get_name_or_typenames<engine_list>(); 
		
	if (vm.count("engine"))
	{
		engine_params = vm["engine"].as< vector<string> >();

		// list of engines
		if (engine_params.size() == 1 && (engine_params[0] == "help" || engine_params[0] == "h"))
		{
			std::ostringstream oss;
			const size_t indent_width = 5;

			BOOST_FOREACH(string s, engine_names)
			{
				oss << std::setw(indent_width) << "" << s << std::endl;
			}

			cout << oss.str();

			return EXIT_SUCCESS;
		}
	}
	else // all engines
	{
		engine_params = engine_names;
	}

	auto timer_names = qfcl::names::get_names<timer_list>();

	// list of timers
	if (vm.count("timer"))
	{

		if (timer_param == "help" || timer_param == "h")
		{
			using namespace boost::lambda;

			std::ostringstream oss;
			const size_t indent_width = 5;

			mpl::for_each<timer_list>( named_functor(oss, indent_width) );

			cout << oss.str();

			return EXIT_SUCCESS;
		}
		else
		{
            if (find(QFCL_NONMEMBER_BEGIN(timer_names), QFCL_NONMEMBER_END(timer_names), timer_param) == QFCL_NONMEMBER_END(timer_names))
				throw po::invalid_option_value(timer_param + " is not a valid timer.");
		}
	}
#ifndef	SYSTEM_CPU_FREQUENCY
    double cpu_frequency = qfcl::timer::get_cpu_frequency();
#else
	double cpu_frequency = SYSTEM_CPU_FREQUENCY;
#endif	// SYSTEM_CPU_FREQUENCY

    cout << boost::format("CPU frequency: %|1$.5| GHz\n\n") % (cpu_frequency / qfcl::timer::one_second);

	cout << qfcl::io::custom_formatted(iterations) << " iterations per engine:" << endl << endl;

#ifdef INCLUDE_UNNAMED
	perform_speed_test<engine_list, CounterType, NAME_OR_TYPENAME>(engine_params, timer_param, iterations, cpu_frequency);
#else
	perform_speed_test<engine_list, CounterType, NAME>(engine_params, timer_param, iterations, cpu_frequency);
#endif // INCLUDE_UNNAMED

	cout << "Press Enter to exit.";

	char c;
	cin.get(c);
}
