/* qfcl/utility/tmp.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef	QFCL_UTILITY_TMP_HPP
#define QFCL_UTILITY_TMP_HPP

/*! \file tmp.hpp
	\brief Some metafunctions for TMP.

	\author James Hirschorn
	\date March 4, 2012
*/

#include <iostream>
#include <string>
#include <vector>

#include <boost/mpl/apply.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/bind.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/insert_range.hpp>
#include <boost/mpl/list_c.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/string.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/void.hpp>
#include <boost/mpl/aux_/na_fwd.hpp>
#include <boost/utility/enable_if.hpp>

#include "type_traits.hpp"

// alias
namespace mpl = boost::mpl;

namespace qfcl {

//! declarations for metaprogramming 
namespace tmp {

//! \cond
//! a reserved class used as a substitute for variadic templates, using partial specialization
class reserved {};
//! \endcond

/*! \defgroup TMP miscellaneous template metaprogramming metafunctions
	@{
*/

/*! \defgroup sequences miscellaneous sequences
	@{
*/
typedef mpl::list_c<int> empty_list;
typedef mpl::vector<> empty_vector;
typedef mpl::clear< mpl::string<0> > empty_string;
//! @}

//! A singletone. <tt>T</tt> is a front extensible sequence.
template<typename T>
struct singleton
	: mpl::vector<T>
{
};

/* concatenation */

//! \cond
struct concatenate0
	: mpl::void_
{
};
template<typename T1>
struct concatenate1
	: T1
{
};
template<typename T1, typename T2>
struct concatenate2
	: mpl::insert_range< T2, typename mpl::begin<T2>::type, T1 >
{
};
template<typename T1, typename T2, typename T3>
struct concatenate3
	: concatenate2< typename concatenate2<T1, T2>::type, T3 >
{
};
template<typename T1, typename T2, typename T3, typename T4>
struct concatenate4
	: concatenate2< typename concatenate3<T1, T2, T3>::type, T4 >
{
};
template<typename T1, typename T2, typename T3, typename T4, typename T5>
struct concatenate5
	: concatenate2< typename concatenate4<T1, T2, T3, T4>::type, T5 >
{
};
//! \endcond

/*! \brief concatenate the arguments into a single sequence

	Variadic metafunction <tt>concatenate<a\e 1,...,a\e n></tt>. 
	Each \c a\e i is a sequence, and the result is a sequence that is the concentation.
*/
template<typename T1 = reserved, typename T2 = reserved, typename T3 = reserved, typename T4 = reserved, typename T5 = reserved>
struct concatenate
	: concatenate5<T1, T2, T3, T4, T5>
{
};

//! \cond
template<>
struct concatenate<reserved, reserved, reserved, reserved, reserved>
	: concatenate0
{
};
template<typename T1>
struct concatenate<T1, reserved, reserved, reserved, reserved>
	: concatenate1<T1>
{
};
template<typename T1, typename T2>
struct concatenate<T1, T2, reserved, reserved, reserved> 
	: concatenate2<T1, T2>
{
};
template<typename T1, typename T2, typename T3>
struct concatenate<T1, T2, T3, reserved, reserved>
	: concatenate3<T1, T2, T3>
{
};
template<typename T1, typename T2, typename T3, typename T4>
struct concatenate<T1, T2, T3, T4, reserved>
{
};
//template<typename T1, typename T2, typename T3, typename T4, typename T5>
//struct concatenate<T1, T2, T3, T4, T5>
//	: concatenate5<T1, T2, T3, T4, T5>
//{
//};

//! \endcond


//! flattens a sequence of pairs into a list of elements
template<typename T>
struct list_of_flattened_pairs
	: mpl::reverse_fold< T, empty_list, 
						 mpl::push_front< mpl::push_front< mpl::_1, mpl::second<mpl::_2> >, 
										  mpl::first<mpl::_2> > >
{
};

//! flattens a sequence of pairs into a vector of elements
template<typename T>
struct vector_of_flattened_pairs
	: mpl::reverse_fold< T, empty_vector, 
						 mpl::push_front< mpl::push_front< mpl::_1, mpl::second<mpl::_2> >, 
										  mpl::first<mpl::_2> > >
{
};

//! whether \c elem is the first element of \c sequence
template<typename Sequence, typename Elem>
struct is_first
	: boost::is_same< typename mpl::front<Sequence>::type, Elem >
{
};

/// NOTE: To be removed.
/*! \brief instantiates a List MetaClass with a type

	A <tt>List MetaClass</tt> is a struct with a type <tt>list</tt> that is a Sequence,
	and a type <tt>name</tt> of type mpl::bool_<bool> indicating whether all elements 
	of <tt>list</tt> should be modeled by the Named concept.
*/
template<typename ListClass, typename T>
struct InstantiateList
{
	typedef typename mpl::transform< typename ListClass::list, mpl::apply1<mpl::_1, T> >::type list;
	typedef typename ListClass::named named;
};

template<typename Seq, typename T>
struct Instantiate
	: mpl::transform< Seq, mpl::apply<mpl::_1, T> >
{};

}}   // qfcl::tmp

// does not work
/* bind a lambda expression */

namespace boost {
namespace mpl {

/// NOTE: Will not work properly if BOOST_MPL_LIMIT_METAFUNCTION_ARITY is increased.

//template<typename Lambda, typename T1 = na, typename T2 = na, typename T3 = na, typename T4 = na, typename T5 = na>
//struct bind_lambda_expression
//        : bind<typename lambda<Lambda>::type, T1, T2, T3, T4, T5>
//{};

template<typename Lambda, typename T1, typename T2>
struct bind_lambda_expression2
        : bind<typename lambda<Lambda>::type, T1, T2>
{};

}}  // boost::mpl

namespace qfcl {
namespace tmp {

/* print_sequence */

namespace detail {

struct print_type
{
	print_type(std::ostream & _os) : os(_os) {}

	template<typename T>
	std::ostream & operator()(T) const
	{
		using namespace std;

		return os << typeid(T).name();
	}
private:
	std::ostream & os;
};

struct print_element
{
    print_element(std::ostream & _os) : os(_os) {}

    template<typename El>
    std::ostream & operator()(El e)
    {
        helper<mpl::is_sequence<El>::value, qfcl::traits::is_integral_constant<El>::value, true> h(os);
        return h(e);
    }

    template<bool is_sequence, bool is_integral, bool is_first>
    struct helper
    {
    private:
        std::ostream & os;
    };
private:
    std::ostream & os;
};

template<size_t n>
std::ostream & print_separator(std::ostream & os)
{
    return os << ", ";
}

template<>
std::ostream & print_separator<1>(std::ostream & os)
{
    return os;
}

template<size_t n>
struct print_subsequence
{
    print_subsequence(std::ostream & _os) : os(_os) {}

    template<typename Seq>
    std::ostream & operator()(Seq S)
    {
        print_element::helper<true, false, false> pe(os);
        return pe(S);
    }
private:
    std::ostream & os;
};

template<>
struct print_subsequence<0>
{
    print_subsequence(std::ostream & _os) : os(_os) {}

    template<typename Seq>
    std::ostream & operator()(Seq)
    {
        return os << ')';
    };
private:
    std::ostream & os;
};

template<>
struct print_element::helper<true, false, false>
{
	helper(std::ostream & _os) : os(_os) {}

	template<typename Seq>
	std::ostream & operator()(Seq e)
	{
		using namespace std;

		typedef typename mpl::front<Seq>::type next_el;
			
		print_element pe(os);
		pe( next_el() );

        static const auto n = mpl::size<Seq>::value;
        print_separator<n>(os);
		print_subsequence<n - 1> f(os);
        typedef typename mpl::if_< mpl::less< mpl::int_<n>, mpl::int_<1> >, qfcl::tmp::empty_vector,
            typename mpl::iterator_range<
                typename mpl::next<typename mpl::begin<Seq>::type>::type, typename mpl::end<Seq>::type
            >
        >::type subsequence;
		return f( subsequence() );
	}
private:
	std::ostream & os;
};

template<>
struct print_element::helper<true, false, true>
{
	helper(std::ostream & _os) : os(_os) {}

	template<typename Seq>
	std::ostream & operator()(Seq S)
	{
		os << '(';

		helper<true, false, false> h(os);
		return h(S);
	}
private:
	std::ostream & os;
};

template<bool is_first>
struct print_element::helper<false, true, is_first>
{
	helper(std::ostream & _os) : os(_os) {}

	template<typename El>
	std::ostream & operator()(El)
	{
		return os << El::value;
	}
private:
	std::ostream & os;
};

template<bool is_first>
struct print_element::helper<false, false, is_first>
{
	helper(std::ostream & _os) : os(_os) {}

	template<typename El>
	std::ostream & operator()(El e)
	{
		return print_type(os)(e);
	}
private:
	std::ostream & os;
};

}	// namespace detail

/*! \brief Prints a Sequence. 
	
	For integral_constants, the corresponding constant is printed.
	For types that are neither integral_costants nor Sequences,
	the type name is printed.
	For Sequences (t1, ... tn), the output is:
	(print_sequence(t1), ... print_sequence(tn)).
*/
struct print_sequence
	//: detail::print_element
{
	print_sequence(std::ostream & _os) : os(_os) {} //detail::print_element(os) {}
	template<typename Seq>
	std::ostream & operator()(Seq S) const
	{
		detail::print_element pe(os);
		pe(S);

		return os << std::endl;
	}
private:
	std::ostream & os;
};


//! @}

}	// tmp

}	// qfcl

#endif	// !QFCL_UTILITY_TMP_HPP
