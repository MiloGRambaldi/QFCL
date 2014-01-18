#ifndef QFCL_MC_PRICER_HPP
#define QFCL_MC_PRICER_HPP

/*! \file qfcl/finance/MC_pricer.hpp
	\brief Monte Carlo pricer

	\author James Hirschorn
	\date Created January 15, 2013
*/

#include <boost/parameter.hpp>

#include "instruments/instrument_base.hpp"

namespace qfcl {
namespace finance {

// AccMarket and AccStatistics must satisfy the Accumulator concept (see boost::accumulators).
// Instr must satisfy out Instrument concept.
template<typename AccMarket, typename AccStatistics, typename Instr>
class MC_pricer
{
	MC_pricer(AccMarket market, AccStatistics stats, Instr instrument)
		: market_(market), stats_(stats), instrument_(instrument)
	{}

	// simulate N scenarios
	template<typename CounterType>
	void simulate(CounterType N)
	{
		for (i = 0; i < N; ++i)
		{
			instrument_.reset();
			market_(dt = instrument_.next());
			stats_(AccMarket::value(market_));
		}
	}
	AccStatistics get_statistics() const {return stats_;}
private:
	AccMarket market_;
	AccStatistics stats_;
	Instr instrument_;
};

}	// namespace finance
}	// namespace qfcl

#endif	// QFCL_MC_PRICER_HPP