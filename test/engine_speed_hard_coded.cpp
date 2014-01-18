#include <functional>
#include <string>
using std::string;

#include <boost/format.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/vector.hpp>
namespace mpl = boost::mpl;
#include <boost/random/mersenne_twister.hpp>
#include <boost/timer/timer.hpp>

#include <qfcl/random/engine/named_adapter.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/utility/comma_separated_number.hpp>
#include <qfcl/utility/names.hpp>
#include <qfcl/utility/type_traits.hpp>

#include "utility/cpu_timer.hpp"

// which TSC to use
typedef rdtscp_timer timer_t;

template<typename CounterType>
void show_timing_results(uint64_t clock_cycles, CounterType iterations, const string & engine_name, double cpu_freq)
{
	// time taken in seconds
	double time_taken = clock_cycles / cpu_freq;

	//std::cerr << "Debug: " << time_taken << "s" << std::endl;

	std::cout 
		<< boost::format("%|1$25|: %|2$13.2f| random numbers/second = %|3$11.8f| nanoseconds/random number = %|4$5.1f| CPU cycles/random number\n\n")
			% engine_name % ( iterations / time_taken ) 
			% ( time_taken * UINT64_C(1000000000) / iterations ) 
			% ( double(clock_cycles) / iterations);
}

// uses time stamp counter
template<typename Engine, typename CounterType>
inline uint64_t time_engine_TSC(Engine & e, CounterType iterations) 
{
	volatile typename Engine::result_type value;

	timer_t timer;

	uint64_t start = timer();

	for (; iterations > 0; --iterations)
	{
		value = e();
	}

	uint64_t end = timer();

	return end - start;
}

#define TIMER_TSC(e, iter, result)				\
	volatile typename Engine::result_type value;\
												\
	timer_t timer;								\
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


template<typename Engine, typename CounterType>
inline void engine_timer(Engine e, CounterType iterations, double cpu_freq, size_t num_output)
{
	for (size_t i = 0; i < num_output; ++i)
	{
		std::cout << boost::format("Random number %1%: %2%\n") % i % e();
	}

	uint64_t result;

	//result = time_engine_TSC(e, iterations);

	TIMER_TSC(e, iterations, result);

	show_timing_results(result, iterations, qfcl::names::name_or_typename(e), cpu_freq);
}

template<typename CounterType>
struct timer_functor
{
	timer_functor(CounterType iter, double cpu_freq, size_t num_output = 0) 
		: iterations_(iter), cpu_freq_(cpu_freq), num_output_(num_output) {}

	template<typename Engine>
	void operator()(Engine e) const
	{
		engine_timer(e, iterations_, cpu_freq_, num_output_);
	}
private:
	CounterType iterations_;
	double cpu_freq_;
	size_t num_output_;
};

int main(int argc, char * argv[])
{
	using namespace std;
	using namespace boost::timer;
	using namespace qfcl::random;

	typedef unsigned long long CounterType;
	//typedef unsigned long CounterType;

	CounterType iterations = 1000000000;
	// For debugging purposes only
	size_t dump_size = 0; //10;

	typedef mpl::vector<boost::random::mt19937, boost_mt19937, mt19937> Engines; 

#ifndef	SYSTEM_CPU_FREQUENCY
	double cpu_frequency = get_cpu_frequency();
#else
	double cpu_frequency = SYSTEM_CPU_FREQUENCY;
#endif	// SYSTEM_CPU_FREQUENCY

	cout << boost::format("CPU frequency: %|1$.5| GHz\n\n") % (cpu_frequency / one_second);

	cout << qfcl::io::custom_formatted(iterations) << " iterations per engine:" << endl << endl;

	//cout << "engine name: " << qfcl::tmp::name_or_typename<boost::random::mt19937>::name() << endl;

	mpl::for_each<Engines>(timer_functor<CounterType>(iterations, cpu_frequency, dump_size));

	/*timer_functor<CounterType> timer(iterations, cpu_frequency);
	boost_mt19937 eng1;
	mt19937 eng2;
	timer(eng1);
	timer(eng2);*/

	cout << "Press Enter to exit.";

	char c;
	cin.get(c);
}
