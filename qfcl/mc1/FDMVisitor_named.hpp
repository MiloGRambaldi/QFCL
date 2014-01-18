/* qfcl/mc1/FDMVisitor_named.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

/*! \file mc1/FDMVisitor_named.hpp
	\brief adds a name tag to each of the FDM schemes

	\author James Hirschorn
	\date September 9, 2012
*/

#ifndef QFCL_MC1_FDM_VISITOR_NAMED_HPP
#define QFCL_MC1_FDM_VISITOR_NAMED_HPP

#include <boost/mpl/string.hpp>

#include "FDMVisitor.hpp"

namespace qfcl {

namespace mc1 {

namespace detail {
	
namespace mpl = boost::mpl;

typedef mpl::string<'E', 'x', 'p', 'l', 'i', 'c', 'i', 't'>::type Explicit_string;
typedef mpl::string<'E', 'u', 'l', 'e', 'r'>::type Euler_string;
typedef mpl::string<'T', 'y', 'p', 'e'>::type Type_string;
typedef mpl::string<'I'>::type Roman_I_string;
typedef qfcl::tmp::concatenate<Roman_I_string, Roman_I_string>::type Roman_II_string;
typedef qfcl::tmp::concatenate< mpl::string<'P', 'r', 'e'>::type, mpl::string<'d', 'i', 'c', 't', 'o', 'r'>::type >::type Predictor_string;
typedef qfcl::tmp::concatenate< mpl::string<'C', 'o', 'r'>::type, mpl::string<'r', 'e', 'c', 't', 'o', 'r'>::type >::type Corrector_string;
typedef mpl::string<'C', 'l', 'a', 's', 's', 'i', 'c', 'o'>::type Classico_string;
typedef qfcl::tmp::concatenate< mpl::string<'R', 'i', 'c', 'h', 'a', 'r', 'd'>::type, mpl::string<'s', 'o', 'n'>::type >::type Richardson_string;
typedef mpl::string<'M', 'i', 'l', 's', 't', 'e', 'i', 'n'>::type Milstein_string;
typedef mpl::string<'K', 'a', 'r', 'h', 'u', 'n', 'e', 'n'>::type Karhunen_string;
typedef mpl::string<'L', 'o', 'e', 'v', 'e'>::type Loeve_string;

typedef qfcl::tmp::concatenate<Explicit_string, Euler_string>::type ExplicitEuler_name;
typedef qfcl::tmp::concatenate<ExplicitEuler_name, Type_string, Roman_II_string>::type ExplicitEulerTypeII_name;
typedef qfcl::tmp::concatenate<ExplicitEuler_name, mpl::string<'M', 'M'>::type>::type ExplicitEulerMM_name;
typedef qfcl::tmp::concatenate<Predictor_string, Corrector_string>::type PredictorCorrector_name;
typedef qfcl::tmp::concatenate<PredictorCorrector_name, Classico_string>::type PredictorCorrectorClassico_name;
typedef qfcl::tmp::concatenate<Richardson_string, Euler_string>::type RichardsonEuler_name;
typedef Milstein_string Milstein_name;
typedef qfcl::tmp::concatenate<Karhunen_string, Loeve_string>::type KarhunenLoeve_name;
typedef qfcl::tmp::concatenate<PredictorCorrector_name, mpl::string<'K', 'L'>::type>::type PredictorCorrectorKL_name;

}	// namespace detail

template<typename X, typename Time, typename RT, typename Generator>
class ExplicitEuler_named
	: public qfcl::named_adapter< ExplicitEuler<X, Time, RT, Generator>, detail::ExplicitEuler_name >
{
public:
	ExplicitEuler_named() {}
	ExplicitEuler_named(long NSteps, Sde<X, Time, RT> & sde, const Generator & generator)
		: qfcl::named_adapter< ExplicitEuler<X, Time, RT, Generator>, 
							   detail::ExplicitEuler_name >(NSteps, sde, generator) {}
};

template<typename X, typename Time, typename RT, typename Generator>
class ExplicitEulerTypeII_named
	: public qfcl::named_adapter< ExplicitEulerTypeII<X, Time, RT, Generator>, detail::ExplicitEulerTypeII_name >
{
public:
	ExplicitEulerTypeII_named() {}
	ExplicitEulerTypeII_named(long NSteps, Sde<X, Time, RT> & sde, const Generator & generator)
		: qfcl::named_adapter< ExplicitEulerTypeII<X, Time, RT, Generator>, 
							   detail::ExplicitEulerTypeII_name >(NSteps, sde, generator) {}
};

template<typename X, typename Time, typename RT, typename Generator>
class ExplicitEulerMM_named
	: public qfcl::named_adapter< ExplicitEulerMM<X, Time, RT, Generator>, detail::ExplicitEulerMM_name >
{
public:
	ExplicitEulerMM_named() {}
	ExplicitEulerMM_named(long NSteps, Sde<X, Time, RT> & sde, const Generator & generator)
		: qfcl::named_adapter< ExplicitEulerMM<X, Time, RT, Generator>, 
							   detail::ExplicitEulerMM_name >(NSteps, sde, generator) {}
};

template<typename X, typename Time, typename RT, typename Generator>
class RichardsonEuler_named
	: public qfcl::named_adapter< RichardsonEuler<X, Time, RT, Generator>, detail::RichardsonEuler_name >
{
public:
	RichardsonEuler_named() {}
	RichardsonEuler_named(long NSteps, Sde<X, Time, RT> & sde, const Generator & generator)
		: qfcl::named_adapter< RichardsonEuler<X, Time, RT, Generator>, 
							   detail::RichardsonEuler_name >(NSteps, sde, generator) {}
};

//! For now alpha = beta = 0.5 is hard-coded. This is a kludge.
template<typename X, typename Time, typename RT, typename Generator>
class PredictorCorrector_named
	: public qfcl::named_adapter< PredictorCorrector<X, Time, RT, Generator>, detail::PredictorCorrector_name >
{
public:
	PredictorCorrector_named() {}
	PredictorCorrector_named(long NSteps, Sde<X, Time, RT> & sde, const Generator & generator)
		: qfcl::named_adapter< PredictorCorrector<X, Time, RT, Generator>, 
							   detail::PredictorCorrector_name >(NSteps, sde, generator, 0.5, 0.5) {}
};

//! For now alpha = beta = 0.5 is hard-coded. This is a kludge.
template<typename X, typename Time, typename RT, typename Generator>
class PredictorCorrectorClassico_named
	: public qfcl::named_adapter< PredictorCorrectorClassico<X, Time, RT, Generator>, 
								  detail::PredictorCorrectorClassico_name >
{
public:
	PredictorCorrectorClassico_named() {}
	PredictorCorrectorClassico_named(long NSteps, Sde<X, Time, RT> & sde, const Generator & generator)
		: qfcl::named_adapter< PredictorCorrectorClassico<X, Time, RT, Generator>, 
							   detail::PredictorCorrectorClassico_name >(NSteps, sde, generator, 0.5, 0.5) {}
};

template<typename X, typename Time, typename RT, typename Generator>
class Milstein_named
	: public qfcl::named_adapter< Milstein<X, Time, RT, Generator>, detail::Milstein_name >
{
public:
	Milstein_named() {}
	Milstein_named(long NSteps, Sde<X, Time, RT> & sde, const Generator & generator)
		: qfcl::named_adapter< Milstein<X, Time, RT, Generator>, 
							   detail::Milstein_name >(NSteps, sde, generator) {}
};

//! kludge: hard-coded tol = 0.01
template<typename X, typename Time, typename RT, typename Generator>
class KarhunenLoeve_named
	: public qfcl::named_adapter< KarhunenLoeve<X, Time, RT, Generator>, detail::KarhunenLoeve_name >
{
public:
	KarhunenLoeve_named() {}
	KarhunenLoeve_named(long NSteps, Sde<X, Time, RT> & sde, const Generator & generator)
		: qfcl::named_adapter< KarhunenLoeve<X, Time, RT, Generator>, 
							   detail::KarhunenLoeve_name >(NSteps, sde, generator, 0.01) {}
};

//! For now alpha = beta = 0.5 and tol are hard-coded. This is a kludge.
template<typename X, typename Time, typename RT, typename Generator>
class PredictorCorrectorKL_named
	: public qfcl::named_adapter< PredictorCorrectorKL<X, Time, RT, Generator>, detail::PredictorCorrectorKL_name >
{
public:
	PredictorCorrectorKL_named() {}
	PredictorCorrectorKL_named(long NSteps, Sde<X, Time, RT> & sde, const Generator & generator)
		: qfcl::named_adapter< PredictorCorrectorKL<X, Time, RT, Generator>, 
							   detail::PredictorCorrectorKL_name >(NSteps, sde, generator, 0.5, 0.5, 0.01) {}
};

}	// namespace mc1

}	// namespace qfcl

#endif  // QFCL_MC1_FDM_VISITOR_NAMED_HPP