#ifndef QFCL_BARRIER_OPTION_HPP
#define QFCL_BARRIER_OPTION_HPP

/*! \file qfcl/finance/instruments/barrier_option.hpp
	\brief barrier options

	\author James Hirschorn
	\date Created January 15, 2013
*/

#include <boost/parameter.hpp>

#include "instrument_base.hpp"

namespace qfcl {
namespace finance {

BOOST_PARAMETER_NAME(upper_barrier)
BOOST_PARAMETER_NAME(lower_barrier)
BOOST_PARAMETER_NAME(up)
BOOST_PARAMETER_NAME(down)
BOOST_PARAMETER_NAME(payoff)
BOOST_PARAMETER_NAME(t0)
BOOST_PARAMETER_NAME(dates)

namespace detail {

template<typename T, typename F>
class barrier_option_imp : public instrument_base
{
public:
	enum hit_behaviour {IN, OUT, NOTHING};

	// temporary solution
	void reset() {date = 0;}
	//date_type first_date() const {return dates_[0];}
	bool is_mature() const {return date >= num_dates;}
	T next() 
	{
		T result = date == 0 ? dates[0] - t0 : dates[date] - dates[date - 1];
		++date;
		return result;
	}
protected:
	template<typename Arg>
	barrier_option_imp(const Arg & args) :
		H(args[_upper_barrier] | T()),
		L(args[_lower_barrier] | T()),
		up(args[_up] | NOTHING),
		down(args[_down] | NOTHING),
		payoff(args[_payoff]),
		t0(args[_t0]),
		dates(args[_dates])
	{
	}

	T H, L;
	hit_behaviour up, down;
	F payoff;
	date_type dates[];
	date_type t0;
	size_t num_dates;
	size_t date;
};

}	// namespace detail

template<typename T, typename F>
class barrier_option : public detail::barrier_option_imp<T, F>
{
public:
	BOOST_PARAMETER_CONSTRUCTOR(
		barrier_option, 
		(detail::barrier_option_imp<T, F>), 
		tag, 
		( required 
			( payoff, (F) )
			( t0, (date_type) )
			( dates, (date_type[]) )
		)
		( optional 
			( upper_barrier, (T) ) 
			( lower_barrier, (T) )
			( up, (hit_behaviour) )
			( down, (hit_behaviour) )
		)
	)
};

}	// namespace finance
}	// namespace qfcl

#endif	// QFCL_BARRIER_OPTION_HPP