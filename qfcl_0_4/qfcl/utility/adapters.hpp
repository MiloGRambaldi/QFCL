/* qfcl/utility/adapters.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_ADAPTERS_HPP
#define QFCL_ADAPTERS_HPP

/*! \file qfcl/utility/adapters.hpp
	\brief Various adapters.

	\author James Hirschorn
	\date September 2, 2012
*/

#include <utility>

#include <boost/mpl/deref.hpp>

namespace qfcl {

/*! \brief Adapts any model to one additionally satisfying the Named concept. 
	
	In other words, it adds a name tag to the type.
*/

template<typename Model, typename Name>
struct named_adapter : public Model
{
	// use perfect forwarding for constructors
	named_adapter() : Model() {}

	template<typename T1>
	explicit named_adapter(T1 && a1) 
		: Model( std::forward<T1>(a1) ) {}

	template<typename T1, typename T2>
	named_adapter(T1 && a1, T2 && a2) 
		: Model( std::forward<T1>(a1), std::forward<T2>(a2) ) {}

	template<typename T1, typename T2, typename T3>
	named_adapter(T1 && a1, T2 && a2, T3 && a3) 
		: Model( std::forward<T1>(a1), std::forward<T2>(a2), std::forward<T3>(a3) ) {}

	template<typename T1, typename T2, typename T3, typename T4>
	named_adapter(T1 && a1, T2 && a2, T3 && a3, T4 && a4) 
		: Model( std::forward<T1>(a1), std::forward<T2>(a2), std::forward<T3>(a3), std::forward<T4>(a4) ) {}

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	named_adapter(T1 && a1, T2 && a2, T3 && a3, T4 && a4, T5 && a5) 
		: Model( std::forward<T1>(a1), std::forward<T2>(a2), std::forward<T3>(a3), std::forward<T4>(a4), std::forward<T5>(a5) ) {}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	named_adapter(T1 && a1, T2 && a2, T3 && a3, T4 && a4, T5 && a5, T6 && a6) 
		: Model( std::forward<T1>(a1), std::forward<T2>(a2), std::forward<T3>(a3), std::forward<T4>(a4), 
			     std::forward<T5>(a5), std::forward<T6>(a6) ) {}

	typedef Name name;
};


/*! \brief traits class to determine whether a type models the Named concept

	Taken from p. 106 of C++ Templates: The Complete Guide.
*/
/*
template<typename T>
struct adapter_traits
{
	static bool is_named() {return sizeof(qfcl::detail::test<T>(0)) == 1;}
};

//! #define version is more powerful (was the actual example mentioned above)
#define QFCL_IS_NAMED(T) (sizeof(qfcl::detail::test<T>(0)) == 1)

//! whether the first element of a sequence is named
#define QFCL_FIRST_IS_NAMED(S) \
	( QFCL_IS_NAMED(typename boost::mpl::deref< typename boost::mpl::begin<S>::type >::type) )
*/
}	// namespace qfcl

#endif  // QFCL_ADAPTERS_HPP
