#ifndef QFCL_INSTRUMENT_BASE_HPP
#define	QFCL_INSTRUMENT_BASE_HPP

/*! \file qfcl/finance/instruments/instrument_base.hpp
	\brief base class for all instruments

	\author James Hirschorn
	\date Created January 15, 2013
*/

#include <boost/date_time/gregorian/gregorian.hpp>

namespace qfcl {
namespace finance {
	
typedef boost::gregorian::date date_type;

struct instrument_base
{
};

}	// namespace finance
}	// namespace qfcl

#endif	// QFCL_INSTRUMENT_BASE_HPP