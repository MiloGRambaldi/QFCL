/* boost random_speed.cpp performance measurements
 *
 * Copyright Jens Maurer 2000
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * $Id: //depot/desrad/main/sw/libs/Random123/boost/libs/random/test/random_speed.cpp#6 $
 */

#include <iostream>
#include <cstdlib>
#include <string>
#include <boost/config.hpp>
#include <boost/random.hpp>
#include <boost/progress.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/random/philox.hpp>
#include <boost/random/threefry.hpp>
#include <boost/random/ars.hpp>
#include <boost/random/aes.hpp>
#include <boost/random/counter_based_engine.hpp>

/*
 * Configuration Section
 */

// define if your C library supports the non-standard drand48 family
//#define HAVE_DRAND48

// define if you have the original mt19937int.c (with commented out main())
#undef HAVE_MT19937INT_C

// define if you have the original mt19937ar.c (with commented out main())
// #define HAVE_MT19937AR_C

// set to your CPU frequency
static const double cpu_frequency = 3.07 * 1e9;

/*
 * End of Configuration Section
 */

/*
 * General portability note:
 * MSVC mis-compiles explicit function template instantiations.
 * For example, f<A>() and f<B>() are both compiled to call f<A>().
 * BCC is unable to implicitly convert a "const char *" to a std::string
 * when using explicit function template instantiations.
 *
 * Therefore, avoid explicit function template instantiations.
 */

// provides an "identity" prf, just for comparison
template <unsigned _N, typename UINT>
struct IdentityPrf{
    static const unsigned N=_N;
    typedef boost::array<UINT, N> domain_type;
    typedef boost::array<UINT, N> range_type;
    typedef boost::array<UINT, 1> key_type;
    typedef UINT dvalue_type;
    typedef UINT rvalue_type;
    typedef domain_type domain_array_type;
    static dvalue_type domain_array_min()  { return std::numeric_limits<dvalue_type>::min(); }
    static dvalue_type domain_array_max()  { return std::numeric_limits<dvalue_type>::max(); }
    static domain_type make_domain(domain_array_type da){ return da; }
    
    typedef range_type range_array_type;
    static rvalue_type range_array_min()  { return std::numeric_limits<rvalue_type>::min(); }
    static rvalue_type range_array_max()  { return std::numeric_limits<rvalue_type>::max(); }
    static range_array_type make_range_array(range_type r) {return r;}

    range_type operator()(domain_type c){ return c; }
};

// provides a run-time configurable linear congruential generator, just
// for comparison
template<class IntType>
class linear_congruential
{
public:
  typedef IntType result_type;

  BOOST_STATIC_CONSTANT(bool, has_fixed_range = false);

  linear_congruential(IntType x0, IntType a, IntType c, IntType m)
    : _x(x0), _a(a), _c(c), _m(m) { }
  // compiler-generated copy ctor and assignment operator are fine
  void seed(IntType x0, IntType a, IntType c, IntType m)
    { _x = x0; _a = a; _c = c; _m = m; }
  void seed(IntType x0) { _x = x0; }
  result_type operator()() { _x = (_a*_x+_c) % _m; return _x; }
  result_type min BOOST_PREVENT_MACRO_SUBSTITUTION () const { return _c == 0 ? 1 : 0; }
  result_type max BOOST_PREVENT_MACRO_SUBSTITUTION () const { return _m -1; }

private:
  IntType _x, _a, _c, _m;
};


// simplest "random" number generator possible, to check on overhead
class counting
{
public:
  typedef int result_type;

  BOOST_STATIC_CONSTANT(bool, has_fixed_range = false);

  counting() : _x(0) { }
  result_type operator()() { return ++_x; }
  result_type min BOOST_PREVENT_MACRO_SUBSTITUTION () const { return 1; }
  result_type max BOOST_PREVENT_MACRO_SUBSTITUTION () const { return (std::numeric_limits<result_type>::max)(); }

private:
  int _x;
};


// decoration of variate_generator to make it runtime-exchangeable
// for speed comparison
template<class Ret>
class RandomGenBase
{
public:
  virtual Ret operator()() = 0;
  virtual ~RandomGenBase() { }
};

template<class URNG, class Dist, class Ret = typename Dist::result_type>
class DynamicRandomGenerator
  : public RandomGenBase<Ret>
{
public:
  DynamicRandomGenerator(URNG& urng, const Dist& d) : _rng(urng, d) { }
  Ret operator()() { return _rng(); }
private:
  boost::variate_generator<URNG&, Dist> _rng;
};

template<class Ret>
class GenericRandomGenerator
{
public:
  typedef Ret result_type;

  GenericRandomGenerator() { };
  void set(boost::shared_ptr<RandomGenBase<Ret> > p) { _p = p; }
  // takes over ownership
  void set(RandomGenBase<Ret> * p) { _p.reset(p); }
  Ret operator()() { return (*_p)(); }
private:
  boost::shared_ptr<RandomGenBase<Ret> > _p;
};


// start implementation of measuring timing

void show_elapsed(double end, int iter, const std::string & name, size_t bytes_per_iter)
{
  double usec = end/iter*1e6;
  double cycles = usec * cpu_frequency/1e6;
  std::cout << name << ": " 
            << usec*1e3 << " nsec/loop = "
            << cycles << " CPU cycles = "
            << cycles/bytes_per_iter << " CPB"
            << std::endl;
}

#if 0
template<class RNG>
void timing(RNG & rng, int iter, const std::string& name)
{
  // make sure we're not optimizing too much
  volatile typename RNG::result_type tmp;
  boost::timer t;
  for(int i = 0; i < iter; i++)
    tmp = rng();
  show_elapsed(t.elapsed(), iter, name, sizeof(tmp));
}
#endif

// overload for using a copy, allows more concise invocation
template<class RNG>
void timing(RNG rng, int iter, const std::string& name)
{
  // make sure we're not optimizing too much
  typename RNG::result_type tmp = 0;
  std::cout << "Distrib: ";
  boost::timer t;
  for(int i = 0; i < iter; i++)
    tmp ^= rng();
  show_elapsed(t.elapsed(), iter, name, sizeof(tmp));
  if(tmp==0)
      std::cerr << "The xor is zero.  That's surprising!\n";
}

template<class RNG>
void timing_sphere(RNG rng, int iter, const std::string & name)
{
  std::cout << "Spherical Distrib: ";
  boost::timer t;
  for(int i = 0; i < iter; i++) {
    // the special return value convention of uniform_on_sphere saves 20% CPU
    const std::vector<double> & tmp = rng();
    (void) tmp[0];
  }
  show_elapsed(t.elapsed(), iter, name, rng().size()*sizeof(rng()));
}

template<class RNG>
void run(int iter, const std::string & name, RNG rng)
{
  std::cout << (RNG::has_fixed_range ? "fixed-range " : "");
  // BCC has trouble with string autoconversion for explicit specializations
  
  // make sure we're not optimizing too much
  std::cout << "Engine: ";
  typename RNG::result_type tmp = 0;
  boost::timer t;
  for(int i = 0; i < iter; i++)
    tmp ^= rng();
  show_elapsed(t.elapsed(), iter, name, sizeof(tmp));
  if(tmp==0)
      std::cerr << name << ": The xor is zero.  That's surprising!\n";
}

template <typename Uint, unsigned N>
boost::array<Uint, N>& oppluseq(boost::array<Uint, N>& lhs, const boost::array<Uint, N> rhs){
    for(size_t i=0; i<N; ++i)
        lhs[i] += rhs[i];
    return lhs;
}

template<class Prf>
void runctr(int iter, const std::string & name, Prf prf)
{
    typename Prf::domain_type c = {{}};

    std::cout << "Prf: ";
    boost::timer t;
    typename Prf::range_type sum = {{}};
    for(int i = 1; i <= iter; i++){
        c[0] = i;
        oppluseq<typename Prf::range_type::value_type, Prf::range_type::static_size>(sum, prf(c));
    }
    show_elapsed(t.elapsed(), iter, name,  sizeof(typename Prf::range_type));
    bool allzero = true;
    for(size_t i=0; i<sum.static_size; ++i)
        if(sum[i]!=0) allzero=false;
    if(allzero)
        std::cout << "all zero\n";
}

#if BOOST_HAS_M128i
template<class Prf>
void runctr128(int iter, const std::string & name, Prf prf)
{
    typename Prf::domain_type c = {{_mm_setzero_si128()}};
    __m128i sum = _mm_setzero_si128();

    __m128i one = _mm_set_epi32(0, 0, 0, 1);
    std::cout << "Prf: ";
    boost::timer t;
    for(int i=0; i < iter; i++){
        c[0] = _mm_add_epi64(c[0], one);
        typename Prf::range_type r = prf(c);
        sum = _mm_xor_si128(sum, r[0]);
    }
    show_elapsed(t.elapsed(), iter, name,  sizeof(sum)*Prf::range_type::static_size);
    // _mm_testz_si128 is an SSE4.2-ism
    //if(_mm_testz_si128(sum, sum))
    union{
        __m128i m;
        uint64_t a64[2];
    }u;
    u.m = sum;
    if(u.a64[0]==0 && u.a64[1]==0)
        std::cerr << name << ":  The xor is zero.  That's surprising!\n";
}
#endif

#ifdef HAVE_DRAND48
// requires non-standard C library support for srand48/lrand48
struct lrand48_ {
    static const bool has_fixed_range = false;
    typedef long result_type;
    lrand48_() {
        using namespace std;
        srand48(1);
    }
    result_type operator()() {
        using namespace std;
        return lrand48();
    }
};
#endif

#ifdef HAVE_MT19937INT_C  // requires the original mt19937int.c
extern "C" void sgenrand(unsigned long);
extern "C" unsigned long genrand();

void run(int iter, const std::string & name, float)
{
  sgenrand(4357);
  timing(genrand, iter, name, 0u);
}
#endif

#ifdef HAVE_MT19937AR_C
extern "C" {
void init_genrand(unsigned long s);
unsigned long genrand_int32(void);
}
struct mt19937_c {
    static const bool has_fixed_range = false;
    mt19937_c() {
        init_genrand(5489);
    }
    typedef unsigned long result_type;
    result_type operator()() {
        return genrand_int32();
    }
};
#endif

template<class PRNG, class Dist>
inline boost::variate_generator<PRNG&, Dist> make_gen(PRNG & rng, Dist d)
{
  return boost::variate_generator<PRNG&, Dist>(rng, d);
}

template<class Gen>
void distrib(int iter, const std::string & name, const Gen &)
{
  Gen gen;

  timing(make_gen(gen, boost::random::uniform_int_distribution<>(-2, 4)),
         iter, name + " uniform_int");

  timing(make_gen(gen, boost::random::uniform_smallint<>(-2, 4)),
         iter, name + " uniform_smallint");

  timing(make_gen(gen, boost::random::bernoulli_distribution<>(0.5)),
         iter, name + " bernoulli");

  timing(make_gen(gen, boost::random::geometric_distribution<>(0.5)),
         iter, name + " geometric");

  timing(make_gen(gen, boost::random::binomial_distribution<int>(4, 0.8)),
         iter, name + " binomial");

  timing(make_gen(gen, boost::random::negative_binomial_distribution<int>(4, 0.8)),
         iter, name + " negative_binomial");

  timing(make_gen(gen, boost::random::poisson_distribution<>(1)),
         iter, name + " poisson");


  timing(make_gen(gen, boost::random::uniform_real_distribution<>(-5.3, 4.8)),
         iter, name + " uniform_real");

  timing(make_gen(gen, boost::random::uniform_01<>()),
         iter, name + " uniform_01");

  timing(make_gen(gen, boost::random::triangle_distribution<>(1, 2, 7)),
         iter, name + " triangle");

  timing(make_gen(gen, boost::random::exponential_distribution<>(3)),
         iter, name + " exponential");

  timing(make_gen(gen, boost::random::normal_distribution<>()),
                  iter, name + " normal polar");

  timing(make_gen(gen, boost::random::lognormal_distribution<>()),
         iter, name + " lognormal");

  timing(make_gen(gen, boost::random::chi_squared_distribution<>(4)),
         iter, name + " chi squared");

  timing(make_gen(gen, boost::random::cauchy_distribution<>()),
         iter, name + " cauchy");

  timing(make_gen(gen, boost::random::fisher_f_distribution<>(4, 5)),
         iter, name + " fisher f");

  timing(make_gen(gen, boost::random::student_t_distribution<>(7)),
         iter, name + " student t");

  timing(make_gen(gen, boost::random::gamma_distribution<>(2.8)),
         iter, name + " gamma");

  timing(make_gen(gen, boost::random::weibull_distribution<>(3)),
         iter, name + " weibull");

  timing(make_gen(gen, boost::random::extreme_value_distribution<>()),
         iter, name + " extreme value");

  timing_sphere(make_gen(gen, boost::random::uniform_on_sphere<>(3)),
                iter/10, name + " uniform_on_sphere");
}

template <typename Prf>
void run_cbe(const std::string& name, int iter){
    std::string pfx= "Engine<" + name + ">";
    run(iter, pfx, boost::random::counter_based_engine<Prf>());
}

int main(int argc, char*argv[])
{
  if(argc != 2) {
    std::cerr << "usage: " << argv[0] << " iterations" << std::endl;
    return 1;
  }

  // okay, it's ugly, but it's only used here
  int iter =
#ifndef BOOST_NO_STDC_NAMESPACE
    std::
#endif
    atoi(argv[1]);

#if 0
#if !defined(BOOST_NO_INT64_T) && \
    !defined(BOOST_NO_INCLASS_MEMBER_INITIALIZATION)
  run(iter, "rand48", boost::rand48());
  linear_congruential<boost::uint64_t>
    lcg48(boost::uint64_t(1)<<16 | 0x330e,
          boost::uint64_t(0xDEECE66DUL) | (boost::uint64_t(0x5) << 32), 0xB,
          boost::uint64_t(1)<<48);
  timing(lcg48, iter, "lrand48 run-time");
#endif

#ifdef HAVE_DRAND48
  // requires non-standard C library support for srand48/lrand48
  run(iter, "lrand48", lrand48_());   // coded for lrand48()
#endif

  run(iter, "minstd_rand0", boost::minstd_rand0());
  run(iter, "minstd_rand", boost::minstd_rand());
  run(iter, "ecuyer combined", boost::ecuyer1988());
  run(iter, "kreutzer1986", boost::kreutzer1986());
  run(iter, "taus88", boost::taus88());
  run(iter, "knuth_b", boost::random::knuth_b());

  run(iter, "hellekalek1995 (inversive)", boost::hellekalek1995());

  run(iter, "mt11213b", boost::mt11213b());
  run(iter, "mt19937", boost::mt19937());
#if !defined(BOOST_NO_INT64_T)
  run(iter, "mt19937_64", boost::mt19937_64());
#endif

  run(iter, "lagged_fibonacci607", boost::lagged_fibonacci607());
  run(iter, "lagged_fibonacci1279", boost::lagged_fibonacci1279());
  run(iter, "lagged_fibonacci2281", boost::lagged_fibonacci2281());
  run(iter, "lagged_fibonacci3217", boost::lagged_fibonacci3217());
  run(iter, "lagged_fibonacci4423", boost::lagged_fibonacci4423());
  run(iter, "lagged_fibonacci9689", boost::lagged_fibonacci9689());
  run(iter, "lagged_fibonacci19937", boost::lagged_fibonacci19937());
  run(iter, "lagged_fibonacci23209", boost::lagged_fibonacci23209());
  run(iter, "lagged_fibonacci44497", boost::lagged_fibonacci44497());

  run(iter, "subtract_with_carry", boost::random::ranlux_base());
  run(iter, "subtract_with_carry_01", boost::random::ranlux_base_01());
  run(iter, "ranlux3", boost::ranlux3());
  run(iter, "ranlux4", boost::ranlux4());
  run(iter, "ranlux3_01", boost::ranlux3_01());
  run(iter, "ranlux4_01", boost::ranlux4_01());
  run(iter, "ranlux64_3", boost::ranlux3());
  run(iter, "ranlux64_4", boost::ranlux4());
  run(iter, "ranlux64_3_01", boost::ranlux3_01());
  run(iter, "ranlux64_4_01", boost::ranlux4_01());
  run(iter, "ranlux24", boost::ranlux3());
  run(iter, "ranlux48", boost::ranlux4());

  run(iter, "counting", counting());

#ifdef HAVE_MT19937INT_C
  // requires the original mt19937int.c
  run<float>(iter, "mt19937 original");   // coded for sgenrand()/genrand()
#endif

#ifdef HAVE_MT19937AR_C
  run(iter, "mt19937ar.c", mt19937_c());
#endif
#endif // 0

  std::cout << "Random Number Functors:\n";
  runctr(iter, "Ident2x32", IdentityPrf<2, uint32_t>());
  runctr(iter, "Ident2x64", IdentityPrf<2, uint64_t>());
  runctr(iter, "Ident4x32", IdentityPrf<4, uint32_t>());
  runctr(iter, "Ident4x64", IdentityPrf<4, uint64_t>());

  runctr(iter, "ars4x32", boost::random::ars<uint32_t>());
  runctr(iter, "ars2x64", boost::random::ars<uint64_t>());
  runctr(iter, "aes2x64", boost::random::aes<uint64_t>());
#ifdef BOOST_HAS_M128i
  runctr128(iter, "ars128i", boost::random::ars<__m128i>());
  runctr128(iter, "aes128i", boost::random::aes<__m128i>());
  boost::random::ars<__m128i> swars;
  swars.usehw(false);
  runctr128(iter/30, "ars128i-sw", swars);
  boost::random::aes<__m128i> swaes;
  swaes.usehw(false);
  runctr128(iter/30, "aes128i-sw", swaes);
#endif 

  runctr(iter, "philox2x32", boost::random::philox<2, uint32_t>());
  runctr(iter, "philox2x64", boost::random::philox<2, uint64_t>());
  runctr(iter, "philox4x32", boost::random::philox<4, uint32_t>());
  runctr(iter, "philox4x64", boost::random::philox<4, uint64_t>());

  runctr(iter, "threefry2x32", boost::random::threefry<2, uint32_t>());
  runctr(iter, "threefry2x64", boost::random::threefry<2, uint64_t>());
  runctr(iter, "threefry4x32", boost::random::threefry<4, uint32_t>());
  runctr(iter, "threefry4x64", boost::random::threefry<4, uint64_t>());

  // Safety margin?  Who needs it...
  std::cout << "Crush-resistant with *NO* safety margin:\n";
  runctr(iter, "ars4x32-5", boost::random::ars<uint32_t, 5>());
  runctr(iter, "ars2x64-5", boost::random::ars<uint64_t, 5>());
#ifdef BOOST_HAS_M128i
  runctr128(iter, "ars128i-5", boost::random::ars<__m128i, 5>());
#endif

  runctr(iter, "philox2x32-7", boost::random::philox<2, uint32_t, 7>());
  runctr(iter, "philox2x64-7", boost::random::philox<2, uint64_t, 7>());
  runctr(iter, "philox4x32-7", boost::random::philox<4, uint32_t, 7>());
  runctr(iter, "philox4x64-7", boost::random::philox<4, uint64_t, 7>());

  runctr(iter, "threefry2x32-13", boost::random::threefry<2, uint32_t, 13>());
  runctr(iter, "threefry2x64-13", boost::random::threefry<2, uint64_t, 13>());
  runctr(iter, "threefry4x32-13", boost::random::threefry<4, uint32_t, 13>());
  runctr(iter, "threefry4x64-13", boost::random::threefry<4, uint64_t, 13>());

  // Engines:
  std::cout << "\nEngines:\n";
  run(iter, "mt19937", boost::random::mt19937());
  run_cbe<IdentityPrf<2, uint64_t> >("Ident2x64", iter);
  run_cbe<boost::random::ars<uint64_t> >("ars2x64", iter);
  run_cbe<boost::random::aes<uint64_t> >("aes", iter);
  run_cbe<boost::random::philox<2, uint64_t> >("philox2x64", iter);
  run_cbe<boost::random::threefry<2, uint64_t> >("threefry2x64", iter);

#if 0
  distrib(iter, "counting", counting());

  distrib(iter, "minstd_rand", boost::minstd_rand());

  distrib(iter, "kreutzer1986", boost::kreutzer1986());

  distrib(iter, "mt19937", boost::mt19937());
  
  distrib(iter, "lagged_fibonacci607", boost::lagged_fibonacci607());
#endif
}
