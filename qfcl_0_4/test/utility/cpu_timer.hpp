/* test/utility/cpu_timer.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt)
 */

/*! \file test/utility/cpu_timer.hpp
	\brief read appropriate clock counter

	\author James Hirschorn
	\date July 7, 2012
*/

#include <boost/mpl/string.hpp>
#include <boost/timer/timer.hpp>

#include <qfcl/utility/tmp.hpp>

/// NOTE: should remove use of ASMLIB?
#ifdef	QFCL_USE_ASMLIB
#include <asmlib.h>
#else
#ifdef _WIN32
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#pragma intrinsic(__rdtscp)
#endif	// _WIN32
#endif  // QFCL_USE_ASMLIB

namespace qfcl {

namespace timer {

// # of nanoseconds in a second
const boost::timer::nanosecond_type one_second = UINT64_C(1000000000);

#if !defined(SYSTEM_CPU_FREQUENCY) && !defined(QFCL_USE_ASMLIB)
#if defined(_WIN32)
static inline uint64_t rdtsc()
{
	return __rdtsc();
}
#ifdef QFCL_RDTSCP
static inline uint64_t rdtscp(uint32_t & aux)
{
    return __rdtscp(aux);
}
#endif // QFCL_RDTSCP
#elif defined(__i386)
static inline uint64_t rdtsc()
{
    uint64_t x;
    asm volatile ("rdtsc" : "=A"(x));
    return x;
}
#ifdef QFCL_RDTSCP
static inline uint64_t rdtscp(uint32_t & aux)
{
    uint64_t x;
    asm volatile ("rdtscp" : "=A"(x), "=c"(aux));
    return x;
}
#endif // QFCL_RDTSCP
#elif defined(__x86_64)
static inline uint64_t rdtsc()
{
    uint32_t hi, lo;
    asm volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return static_cast<uint64_t>(lo) | ( static_cast<uint64_t>(hi)<<32 );
}
#ifdef QFCL_RDTSCP
static inline uint64_t rdtscp(uint32_t & aux)
{
    uint32_t hi, lo;
    asm volatile ("rdtscp" : "=a"(lo), "=d"(hi), "=c"(aux));
    return static_cast<uint64_t>(lo) | ( static_cast<uint64_t>(hi)<<32 );
}
#endif // QFCL_RDTSCP
#endif
//! \cond
namespace detail {

//! alias
namespace mpl = boost::mpl;

typedef mpl::string<'R', 'D', 'T', 'S', 'C'>::type RDTSC_name;

typedef qfcl::tmp::concatenate<RDTSC_name, mpl::string<'P'>::type>::type RDTSCP_name;
}	// namespace detail
//! \endcond

//! wrapper to ensure inlining
struct rdtsc_timer
{
    uint64_t operator()() {return rdtsc();}

	typedef detail::RDTSC_name name;
};

#ifdef QFCL_RDTSCP
struct rdtscp_timer
{
    static uint32_t ui;

    uint64_t operator()() {return rdtscp(&ui);}

	typedef detail::RDTSCP_name name;
};

uint32_t rdtscp_timer::ui;
#endif // QFCL_RDTSCP
#endif

#ifndef SYSTEM_CPU_FREQUENCY 
//! cpu frequency in clocks per second
double get_cpu_frequency()
{
	using namespace boost::timer;

	//uint64_t start = rdtsc();

	cpu_timer timer;

	//uint64_t ctor = rdtsc() - start;

    uint64_t time_stamp_counter_start = rdtsc();

	while ( timer.elapsed().wall < one_second );

	timer.stop();

	//std::cerr << "cpu_timer ctor clocks: " << ctor << std::endl;
	
    uint64_t time_stamp_counter_end = rdtsc();

	return double( (time_stamp_counter_end - time_stamp_counter_start) * one_second )
		/ ( timer.elapsed().wall );
}
#endif // !SYSTEM_CPU_FREQUENCY

}   // namespace timer

}   // namespace qfcl
