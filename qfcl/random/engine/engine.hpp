#ifndef QFCL_RANDOM_ENGINE_HPP
#define QFCL_RANDOM_ENGINE_HPP

namespace qfcl {

namespace random {
	
//! tag struct for all random engines
struct random_engine_tag {};

//! traits class for all random engines
template<typename EngineTag, typename UIntType_>
struct engine_traits
{
	typedef EngineTag	engine_category;
    typedef UIntType_	UIntType;
};

}	// namespace random

}	// namespace qfcl

#endif	// QFCL_RANDOM_ENGINE_HPP
