#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/timer/timer.hpp>

//#include <Eigen/Dense>

#include <qfcl/random/engine/mersenne_twister.hpp>

const std::string usage = "Usage: matrix_power_speed [exponentsize1 exponentsize2 ...]\n";
const std::string description = "Comparison between QFCL and NTL F_2-matrix powers.";

int main(int argc, char * argv[])
{
	using namespace std;
	using namespace boost::timer;
	using namespace qfcl::random;
	
	vector<unsigned long long> exponent_sizes;

	if (argc >= 2)
	{
		for (int i = 1; i < argc; ++i)
			exponent_sizes.push_back( boost::lexical_cast<unsigned long long>(argv[i]) );
	}
	else // print usage info
	{
		cout << usage << endl << description << endl;
		exit(EXIT_SUCCESS);
	}

    typedef qfcl::random::mt19937 Engine;

	typedef NTL::mat_GF2 NTL_matrix;

	NTL_matrix T;
	T = Engine::TransitionMatrix(false);

	BOOST_FOREACH(unsigned long long e, exponent_sizes)
	{
		cpu_timer t_power;

		NTL_matrix NTL_power = NTL::power(T, e);

		t_power.stop();

		cpu_timer t_pow;

		NTL_matrix NTL_pow = qfcl::random::detail::pow(T, e);

		t_pow.stop();

		cout << "Exponent " << e << ": " << (NTL_power == NTL_pow ? "PASSED" : "FAILED") << endl << endl;
		cout << "Time taken for NTL::power: " << t_power.format() << endl;
		cout << "Time taken for pow: " << t_pow.format() << endl << endl;
	}
}
