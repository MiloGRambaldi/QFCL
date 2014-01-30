/* qfcl/utility/for_each_fwd.hpp
 *
 * Copyright (C) 2013 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef	QFCL_FOR_EACH_FWD_HPP
#define QFCL_FOR_EACH_FWD_HPP

/*! \file for_each_fwd.hpp

	\author James Hirschorn
    \date February 21, 2013
*/

//#include <functional>

/*#include <boost/bind.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/bind.hpp>
#include <boost/mpl/empty_sequence.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/identity.hpp>*/
#include <boost/mpl/is_sequence.hpp>
/*
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/lambda.hpp>*/
#include <boost/mpl/limits/arity.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/or.hpp>
/*#include <boost/mpl/pair.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/single_view.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector.hpp>*/
namespace mpl = boost::mpl;

// mpl::is_lambda_expression is broken at this time
#include <boost/tti/detail/dlambda.hpp>
namespace tti = boost::tti::detail;

#include <boost/utility/enable_if.hpp>

//#include <qfcl/utility/tmp.hpp>

namespace qfcl {

namespace tmp {

//! IDENTITY maps <T1...TN> to TN 
struct IDENTITY;

struct INSTANTIATION;

namespace detail {
template<typename T>
struct is_lambda_expression : tti::is_lambda_expression<T> {};
template<typename T>
struct is_not_sequence : mpl::not_<mpl::is_sequence<T>> {};
// This defines what should be treated as an "Operation".
// We copied mpl::transform but is this correct?
template<typename T>
struct is_operator
    : mpl::or_<is_lambda_expression<T>, is_not_sequence<T>> {};
}	// namespace detail

template<typename Sequence1, typename F>
inline 
void for_each(F f);

template<typename Sequence1, typename TransformOp1, typename F>
inline 
void for_each(
	F f, 
	typename boost::enable_if< detail::is_operator<TransformOp1> >::type * dummy = 0
);

template<typename Sequence1, typename Sequence2, typename F>
void for_each(
	F f, 
	typename boost::disable_if< detail::is_operator<Sequence2> >::type * dummy = 0
);

template<typename Sequence1, typename Sequence2, typename TransformOp1, typename TransformOp2, typename F>
void for_each(
	F f,
	typename boost::enable_if<detail::is_operator<TransformOp1>>::type * dummy = 0
);

template<typename Sequence1, typename Sequence2, typename Sequence3, typename F>
void for_each(F f);

template<typename Sequence1, typename Sequence2, typename Sequence3, typename Sequence4, typename F>
void for_each(
	F f,
	typename boost::disable_if<detail::is_operator<Sequence3>>::type * dummy = 0
);

template<typename Sequence1, typename Sequence2, typename Sequence3, typename Sequence4, typename Sequence5, typename F>
void for_each(F f);

// The limit is 5 by default.
#if BOOST_MPL_LIMIT_METAFUNCTION_ARITY >= 6
template<typename Sequence1, typename Sequence2, typename Sequence3, typename Sequence4, typename Sequence5, typename Sequence6, typename F>
void for_each(F f);
#endif

}	// namespace tmp

namespace fusion {

template<typename Sequence1, typename F>
inline
void for_each(Sequence1 & seq, F f);

template<typename Sequence1, typename Sequence2, typename F>
void for_each(Sequence1 & seq1, Sequence2 & seq2, F f);

template<typename Sequence1, typename Sequence2, typename Sequence3, typename F>
void for_each(Sequence1 & seq1, Sequence2 & seq2, Sequence3 & seq3, F f);

template<typename Sequence1, typename Sequence2, typename Sequence3, typename Sequence4, typename F>
void for_each(Sequence1 & seq1, Sequence2 & seq2, Sequence3 & seq3, Sequence4 & seq4, F f);

template<typename Sequence1, typename Sequence2, typename Sequence3, typename Sequence4, typename Sequence5, typename F>
void for_each(Sequence1 & seq1, Sequence2 & seq2, Sequence3 & seq3, Sequence4 & seq4, Sequence5 & seq5, F f);

template<typename Sequence1, typename Sequence2, typename Sequence3, typename Sequence4, typename Sequence5, typename Sequence6, typename F>
void for_each(Sequence1 & seq1, Sequence2 & seq2, Sequence3 & seq3, Sequence4 & seq4, Sequence5 & seq5, Sequence6 & seq6, F f);

}   // namespace fusion

}	// namespace qfcl

#endif // !QFCL_FOR_EACH_FWD_HPP
