#pragma warning(disable:4996)

#include <array>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/timer/timer.hpp>

#include <qfcl/random/engine/counting.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/random/engine/twisted_generalized_feedback_shift_register.hpp>

#include "engine_common.ipp"

//typedef qfcl::random::mt11213a Engine;
typedef qfcl::random::mt19937 Engine;
//typedef qfcl::random::tt800 Engine;
//typedef qfcl::random::micro_mt Engine;
//typedef qfcl::random::counting_uint Engine;

const std::string usage = "Usage: linear_generator_discard_speed [jumpsize1 jumpsize2 ...]\n";
const std::string description = 
	"This tests the performance of jumps of size jumpsizeN for each N, where\n"		\
	"jumpsizeN can be any value representable by a long long. If the transition\n"	\
	"matrix file is present then it reads from the file, otherwise a timed\n"		\
	"computation is made. Similarly for the jump matrix file corresponding to\n"	\
	"jumpsizeN.\n";

int main(int argc, char * argv[])
{
	using namespace std;
	using namespace boost::timer;
	using namespace qfcl::random;

	cout << "Testing discard member function for the " << boost::mpl::c_str<Engine::name>::value << " engine.\n\n";

	vector<long long> jump_sizes;

	if (argc >= 2)
	{
		for (int i = 1; i < argc; ++i)
			jump_sizes.push_back( boost::lexical_cast<long long>(argv[i]) );
	}
	else // print usage info
	{
		cout << usage << endl << description << endl;
		exit(EXIT_SUCCESS);
	}

	Engine eng;
	boost::random::mt19937 boost_eng;

	// compute or read the transition matrix
	Matrix<2> T;

	bool T_onFile = Engine::TransitionMatrix_file_exists();

	if (!T_onFile)
		cout << "Computing transition matrix ..." << endl;

	cpu_timer TM_timer;

	T = Engine::TransitionMatrix();

	TM_timer.stop();
	cout << (T_onFile ? "Time taken to read transition matrix file: "
		: "Time taken to compute transition matrix: ") 
		<< TM_timer.format() << endl;

	// compute or read the inverse of the transition matrix
	Matrix<2> T_inverse;

	bool T_inverse_onFile = Engine::TransitionMatrix_file_exists(true);

	if (!T_inverse_onFile)
		cout << "Computing inverse of the transition matrix ..." << endl;

	cpu_timer TMI_timer;

	T_inverse = Engine::TransitionMatrix(true);

	TMI_timer.stop();
	cout << (T_inverse_onFile ? "Time taken to read inverse transition matrix file: "
		: "Time taken to compute inverse of transition matrix: ") 
		<< TMI_timer.format() << endl;

	BOOST_FOREACH(long long v, jump_sizes)
	{
		cout << "Jump size: " << v << endl;
		
		bool JumpMatrix_onFile = Engine::JumpMatrix_file_exists(v);

		if (!JumpMatrix_onFile)
			cout << "Computing jump matrix ..." << endl;

		cpu_timer jump_timer;

		Engine::JumpMatrix(v);

		jump_timer.stop();

		cout << (JumpMatrix_onFile ? "Time taken to read jump matrix file: "
					: "Time taken to compute jump matrix: ") << jump_timer.format() << endl;

		cpu_timer peek_timer;

		Engine::result_type peek = eng.peek(v + 1);

		peek_timer.stop();

		cout << "QFCL, time taken to peek ahead " << v << " steps: " << peek_timer.format();
	
		cpu_timer discard_timer;

		eng.skip(v);

		discard_timer.stop();

		cout << "QFCL, time taken to discard (skip) " << v << " numbers: " << discard_timer.format();

		if (v >= 0)
		{
			cpu_timer boost_timer;

			boost_eng.discard(v);

			boost_timer.stop();

			cout << "Boost, time taken to discard " << v << " numbers: " << boost_timer.format();
		}

		cout << setw(78) << setfill('-') << ' ' << endl << "Verification:" << endl;

		cout << "QFCL, next number: " << eng() << endl;
		cout << "QFCL, next number via peek: " << peek << endl;
		if (v >= 0)
			cout << "Boost, next number: " << boost_eng() << endl;
		cout << endl;
	}

}