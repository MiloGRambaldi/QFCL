// A tiny example: roll a six-sided die, using the
// uniform_int_distribution, driven by a counter_based_urng
// wrapped around a philox<4,uint32_t> pseudo-random function.

#include <boost/random/counter_based_urng.hpp>
#include <boost/random/philox.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <iostream>

int main(int argc, char **argv){
    typedef boost::random::philox<4, uint32_t> Prf;
    Prf::key_type k={{}};
    Prf::domain_type c={{}};
    // initialize k and c with application-specific, job-specific
    // or thread-specific values, e.g.,
    // k[0] = thread_id;
    // k[1] = seed;
    // ctr[0] = timestamp;
    // ctr[1] = stocksymbol_id;
    boost::random::counter_based_urng<Prf> rng(Prf(k), c);
    boost::random::uniform_int_distribution<int> six(1,6);

    for(int i=0; i<10; ++i){
        std::cout << six(rng) << "\n";
    }
    return 0;
}
