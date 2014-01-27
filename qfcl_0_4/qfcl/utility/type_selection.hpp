/* qfcl/utility/type_selection.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_TYPE_SELECTION_HPP
#define QFCL_TYPE_SELECTION_HPP

/*! \file qfcl/utility/type_selection.hpp
	\brief Run-time type selction for a collection of types, without the need for a common base class.

	\author James Hirschorn
	\date September 3, 2012
*/

#include <string>
#include <typeinfo>
#include <vector>

#include <boost/mpl/bool.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/mpl/string.hpp>
#include <boost/mpl/transform.hpp>

#include <qfcl/utility/for_each.hpp>
#include <qfcl/utility/names.hpp>

#include "adapters.hpp"
#include "tmp.hpp"

namespace qfcl {

namespace type_selection {

	/*
struct DefaultFactory
{
	template<typename T>
	T operator()(T & t) const {return T();}
};

template<typename T1, typename F>
struct instantiated_functor_
{
	instantiated_functor_(T1 & t1_, F & f_) : t1(t1_), f(f_) {}

	template<typename T2>
	void operator()(T2 & t2)
	{
		f(t1, t2);
	}

	//template<typename T2>
	//T2 factory(T2 & t2)
	//{
	//	return f.factory(t1, t2);
	//}
private:
	T1 & t1;
	F & f;
};

template<typename T1, typename F>
instantiated_functor_<T1, F> instantiate_functor(T1 & t1, F & f)
{
	return instantiated_functor_<T1, F>(t1, f);
}

//! Select two types
/*template<typename TypeList1, bool use_name1, typename TypeList2, bool use_name2, typename F>
struct type_selector2
{
//	type_selector2() {}
	type_selector2(const std::vector<std::string> & type_names1_, 
				   const std::vector<std::string> & type_names2_, F & f_)
		: type_names1(type_names1_), type_names2(type_names2_), f(f_) {}

	template<typename T1>
	void operator()(T1 & t1)
	{
		if ( (use_name1 && find(begin(type_names1), end(type_names1), boost::mpl::c_str<T1::name>::value) != end(type_names1)) ||
			 (!use_name1 && find(begin(type_names1), end(type_names1), typeid(T1).name()) != end(type_names1)) )
		{
			typedef mpl::if_< TypeList2::is_PlaceholderExpression, qfcl::tmp::InstantiateList<TypeList2, T1>,
							  TypeList2 >::type DerivedList;

			//BOOST_MPL_ASSERT( (boost::is_same<T1, qfcl::random::boost_mt19937>) );
			//BOOST_MPL_ASSERT( (mpl::equal<InstantiatedList, scheme>) );
			//BOOST_MPL_ASSERT_MSG(false, WHAT, (T1));

			//auto inner_selector = type_selection_creator<DerivedList, use_name2, Factory2>(type_names2, factory2, boost::bind(f, t1, _1)); 
			auto inner_selector = type_selection_creator<DerivedList, use_name2>(type_names2, instantiate_functor(t1, f)); 
			inner_selector.start();
		}
	}

	void start() {boost::mpl::for_each<TypeList1::list>(*this);}
protected:
	F & f;
	std::vector<std::string> type_names1;
	std::vector<std::string> type_names2;
};*/

struct NAME 
{
	template<typename T>
	const char * operator()(T & t) const
	{
		return names::name(t);
	}

	typedef mpl::true_ selection_method;
};

struct TYPENAME 
{
	typedef mpl::true_ selection_method;
};

struct NAME_OR_TYPENAME 
{
	template<typename T>
	const char * operator()(T & t) const
	{
		return names::name_or_typename(t);
	}

	typedef mpl::true_ selection_method;
};

template<typename T, typename Enable = void>
struct is_selection_method : mpl::false_ {};
template<typename T>
struct is_selection_method<T, typename boost::enable_if<typename T::selection_method>::type> : mpl::true_ {};

//namespace type_selection_method {
//
//struct _NAME 
//{
//	template<typename T>
//	const char * operator()(T & t)
//	{
//		return names::name(t);
//	}
//};
//const _NAME NAME;
//
//struct _TYPENAME 
//{
//};
//const _TYPENAME TYPENAME;
//
//struct _NAME_OR_TYPENAME 
//{
//	template<typename T>
//	const char * operator()(T & t)
//	{
//		return names::name_or_typename(t);
//	}
//};
//const _NAME_OR_TYPENAME NAME_OR_TYPENAME;
//
//}	// namespace type_selection_method
//using type_selection_method::NAME;
//using type_selection_method::TYPENAME;
//using type_selection_method::NAME_OR_TYPENAME;

//namespace instantiation_method {
//
//template<size_t n>
//struct _NO_INSTANTIATION {};
//
//template<>
//struct _NO_INSTANTIATION<1>// : mpl::always<mpl::_1>
//{
//	template<typename T>
//	struct apply
//	{
//		//typedef std::vector<T> type;
//		typedef mpl::always<T> type;
//	}; // mpl::always<T> {};
//};
//
//const _NO_INSTANTIATION<1> NO_INSTANTIATION1;
//
//struct _INSTANTIATION
//{
//	template<typename T>
//	struct apply : T {};
//};
//
//const _INSTANTIATION INSTANTIATION;
//
//}	// namespace instatiation_method
//using instantiation_method::NO_INSTANTIATION1;
//using instantiation_method::INSTANTIATION;
//template<size_t n>
//instantiation_method::_NO_INSTANTIATION<n> NO_INSTANTIATION()
//{
//	return instantiation_method::_NO_INSTANTIATION<n>();
//}

namespace detail {

template<typename Method, typename F>
struct for_each_selector_object1 
{
	for_each_selector_object1(
		F f, //F & f, 
		const names::vector_of_strings & selection
		)
		: f_(f), selection_(selection) 
	{};

	typedef void result_type;

	template<typename T>
	void operator()(T t) const
	{
		using namespace std;

        if (find(QFCL_NONMEMBER_BEGIN(selection_), QFCL_NONMEMBER_END(selection_), Method()(t)) != QFCL_NONMEMBER_END(selection_))
			f_(t);
	}
protected:
	mutable F f_; //F & f_;
	const names::vector_of_strings & selection_;
};

//template<typename TypeList1, typename TypeList2, typename Method1, typename Method2, typename Instantiate, typename F>
//struct for_each_selector_object2 {};

template<typename Method1, typename Method2, typename F>// typename Instantiate, typename F>
struct for_each_selector_object2 //<TypeList1, TypeList2, Method1, Method2, mpl::false_, F>
{
	for_each_selector_object2(//F & f, 
		F f,
		const names::vector_of_strings & selection1, const names::vector_of_strings & selection2
		)
		: f_(f), selection1_(selection1), selection2_(selection2)
	{};

	typedef void result_type;

	//BOOST_MPL_ASSERT((boost::is_same<mpl::int_<0>, mpl::int_<1>>));
	//BOOST_MPL_ASSERT((boost::is_same<mpl::apply<Instantiate, T2>::type, mpl::always<T2>>));
	//BOOST_MPL_ASSERT((boost::is_same< mpl::apply<mpl::apply<Instantiate, T2>, T1>::type, T2 >));
	//BOOST_MPL_ASSERT((boost::is_same< mpl::apply<Instantiate, T2>::type, std::vector<T2> >));
	//typedef mpl::int_<0> T;
	//BOOST_MPL_ASSERT((boost::is_same< mpl::apply<Instantiate, T>::type, T >));

	/*template<typename T1>
	struct instantiation :
		tmp::Instantiate<Seq, mpl::apply<Instantiate, T2>
		mpl::apply<typename mpl::apply<Instantiate, T2>::type, T1>
	{};*/

	template<typename T1, typename T2>
	result_type operator()(T1 t1, T2 t2) //const //typename instantiation<T1, T2>::type t2_derived) const
	{
		//typedef mpl::apply<Instantiate, T2>::type instantiation_metafunction;
		////typedef mpl::always<T2> instantiation_metafunction;
		//typedef mpl::apply<instantiation_metafunction, T1>::type T2_Derived;
		////BOOST_MPL_ASSERT((boost::is_same<T2_Derived, T2>));
		//T2_Derived t2_derived;
		////T2 t2_derived;

        if( find(QFCL_NONMEMBER_BEGIN(selection1_), QFCL_NONMEMBER_END(selection1_), Method1()(t1)) != QFCL_NONMEMBER_END(selection1_) &&
            find(QFCL_NONMEMBER_BEGIN(selection2_), QFCL_NONMEMBER_END(selection2_), Method2()(t2)) != QFCL_NONMEMBER_END(selection2_) )
		    f_(t1, t2);
	}
protected:
//	F & f_;
	F f_;
	const names::vector_of_strings & selection1_;
	const names::vector_of_strings & selection2_;
};

}	// namespace detail

/*! \brief Allows for run-type type selection for a collection of types, without the need for a common base class.
		
    By default, NAME method is used.
*/

/* 1-ary */

template<typename TypeList1, typename SelectionMethod1, typename F>
inline
void for_each_selector(const names::vector_of_strings & selection, F f)
{
	qfcl::tmp::for_each<TypeList1>(
		detail::for_each_selector_object1<SelectionMethod1, F>(f, selection)
	);
}

template<typename TypeList1, typename F>
inline
void for_each_selector(
	const names::vector_of_strings & selection, // F & f
	F f
	)
{
    for_each_selector<TypeList1, NAME>(selection, f);
}

/* 2-ary */

template<
	typename TypeList1, typename TypeList2, 
	typename SelectionMethod1, typename SelectionMethod2, 
	typename Transformation1, typename Transformation2, 
	typename F
>
inline
void for_each_selector(
	const names::vector_of_strings & selection1, const names::vector_of_strings & selection2, 
	F f
)
	//F & f)
{
	//typedef Transformation1::apply<char, int>::type r;
	//BOOST_MPL_ASSERT((mpl::is_lambda_expression<tmp::IDENTITY>));
	//BOOST_MPL_ASSERT((mpl::and_<
	//	mpl::is_sequence<TypeList1>, mpl::is_sequence<TypeList2>, 
	//	mpl::is_lambda_expression<Transformation1>, mpl::is_lambda_expression<Transformation2>
	//>));
	qfcl::tmp::for_each<TypeList1, TypeList2, Transformation1, Transformation2>(
		detail::for_each_selector_object2<SelectionMethod1, SelectionMethod2, F>
		(f, selection1, selection2)
	);
}

// default selection methods
template<typename TypeList1, typename TypeList2, typename Transformation1, typename Transformation2, typename F>
inline
void for_each_selector(
	const names::vector_of_strings & selection1, const names::vector_of_strings & selection2, 
	F f
)
					   //F & f)
{
    for_each_selector<TypeList1, TypeList2, NAME, NAME, Transformation1, Transformation2>(selection1, selection2, f);
}

// no transformation
template<
	typename TypeList1, typename TypeList2, 
	typename SelectionMethod1, typename SelectionMethod2, 
	typename F
>
inline
void for_each_selector(
	const names::vector_of_strings & selection1, const names::vector_of_strings & selection2, 
	SelectionMethod1 & method1, SelectionMethod2 & method2,
                       //F & f
	F f
	)
{
    for_each_selector<TypeList1, TypeList2, SelectionMethod1, SelectionMethod2, tmp::IDENTITY, tmp::IDENTITY>(selection1, selection2, f);
}

// default selection methods and Instantiation
template<typename TypeList1, typename TypeList2, typename F>
inline
void for_each_selector(
	const names::vector_of_strings & selection1, const names::vector_of_strings & selection2, 
	//F & f)
	F f
	)
{
    for_each_selector<TypeList1, TypeList2, NAME, NAME>(selection1, selection2, f);
}

// single element selections
template<
	typename TypeList1, typename TypeList2, 
	typename SelectionMethod1, typename SelectionMethod2, 
	typename Transformation1, typename Transformation2, 
	typename F
>
inline
void for_each_selector(
	const std::string & s1, const std::string & s2, 
	F f
	)
					   //F & f)
{
	names::vector_of_strings selection1, selection2;
	selection1.push_back(s1); selection2.push_back(s2);

	for_each_selector<
		TypeList1, TypeList2, 
		SelectionMethod1, SelectionMethod2, 
		Transformation1, Transformation2
	>(selection1, selection2, f);
}

// single element selections, default selection methods
template<
	typename TypeList1, typename TypeList2, 
	typename Transformation1, typename Transformation2, 
	typename F
>
inline
void for_each_selector(
	const std::string & s1, const std::string & s2, 
//	F & f,
	F f,
//	typename boost::enable_if<mpl::and_<
//		mpl::is_lambda_expression<Transformation1>, mpl::is_lambda_expression<Transformation2>
	typename boost::disable_if<mpl::or_<
		is_selection_method<Transformation1>, is_selection_method<Transformation2>
	> >::type * dummy = 0
	)
{
	for_each_selector<TypeList1, TypeList2, NAME_OR_TYPENAME, NAME_OR_TYPENAME, Transformation1, Transformation2>(s1, s2, f);
}

//BOOST_MPL_ASSERT((is_selection_method<NAME>));

// single element selections, no transformations
template<
	typename TypeList1, typename TypeList2, 
	typename SelectionMethod1, typename SelectionMethod2, 
	typename F
>
inline
void for_each_selector(
	const std::string & s1, const std::string & s2, 
//	F & f,
	F f,
	typename boost::enable_if<mpl::and_<
		mpl::is_sequence<TypeList1>, mpl::is_sequence<TypeList2>,
		is_selection_method<SelectionMethod1>, is_selection_method<SelectionMethod2>
	> >::type * dummy = 0
	)
{
	for_each_selector<TypeList1, TypeList2, SelectionMethod1, SelectionMethod2, tmp::IDENTITY, tmp::IDENTITY>(s1, s2, f);
}

// single element selections, default selection methods and no transformations
template<
	typename TypeList1, typename TypeList2, 
	typename F
>
inline
void for_each_selector(
	const std::string & s1, const std::string & s2, 
					  // F & f)
	F f
	)
{
	for_each_selector<TypeList1, TypeList2, tmp::IDENTITY, tmp::IDENTITY>(s1, s2, f);
}

/*
template<typename TypeList1, typename F>
inline
void for_each_selector(const names::vector_of_strings & selection, F & f) //, type_selection_method & method = NAME_OR_TYPENAME)
{
	qfcl::tmp::for_each<TypeList1>(
		detail::for_each_selector_object1<TypeList1, type_selection_method::_NAME_OR_TYPENAME, F>(f, selection)
	);
}

template<typename TypeList1, typename F>
inline
void for_each_selector(const names::vector_of_strings & selection, type_selection_method::_NAME_OR_TYPENAME m, F & f)
{
	for_each_selector<TypeList1>(selection, f);
}

template<typename TypeList1, typename F>
inline
void for_each_selector(const names::vector_of_strings & selection, type_selection_method::_NAME m, F & f)
{
	qfcl::tmp::for_each<TypeList1>(
		detail::for_each_selector_object1<TypeList1, type_selection_method::_NAME, F>(f, selection)
	);
}
*/

/* obsolete 
//! uses name tag type selection whenever F is Named, and default factory
template<typename TypeList, typename F>
type_selector<TypeList, TypeList::named::value, DefaultFactory, F> 
type_selection_creator(const std::vector<std::string> & type_names, F & f)
{
	return type_selection_creator<TypeList, TypeList::named::value, DefaultFactory, F>(type_names, DefaultFactory(), f);
}
*/

//
////! for a single name
//template<typename TypeList, typename F>
//type_selector<TypeList, QFCL_FIRST_IS_NAMED(TypeList), F> 
//type_selection_creator(const std::string & type_name, F & f)
//{
////	cerr << "Debug: " << QFCL_FIRST_IS_NAMED(TypeList) << endl; 
//
//	std::vector<std::string> type_names(1);
//	type_names[0] = type_name;
//
//	return type_selection_creator<TypeList>(type_names, f);
//}

/*
//! factory for two types
template<typename TypeList1, bool use_name1, typename TypeList2, bool use_name2, typename F>
type_selector2<TypeList1, use_name1, TypeList2, use_name2, F>
type_selection_creator2(const std::vector<std::string> & type_names1, 
					    const std::vector<std::string> & type_names2, F & f)
{
	return type_selector2<TypeList1, use_name1, TypeList2, use_name2, F>(type_names1, type_names2, f);
}

//! uses name tag type selection whenever F is Named
template<typename TypeList1, typename TypeList2, typename F>
type_selector2<TypeList1, TypeList1::named::value, TypeList2, TypeList2::named::value, F> 
type_selection_creator2(const std::vector<std::string> & type_names1, const std::vector<std::string> & type_names2, F & f)
{
	return type_selection_creator2<TypeList1, TypeList1::named::value,
								   TypeList2, TypeList2::named::value, F>(type_names1, type_names2, f); 
}

////! single name version
//template<typename TypeList1, typename Factory1, typename TypeList2, typename Factory2, typename F>
//type_selector2<TypeList1, TypeList1::named::value, Factory1, TypeList2, TypeList2::named::value, Factory2, F> 
//type_selection_creator2(const std::string & type_name1, Factory1 & factory1, const std::string & type_name2, Factory2 & factory2, F & f)
//{
//	std::vector<std::string> type_names1(1), type_names2(1);
//	type_names1[0] = type_name1;
//	type_names2[0] = type_name2;
//
//	return type_selection_creator2(const std::string & type_name1, Factory1 & factory1, 
//								   const std::string & type_name2, Factory2 & factory2, F & f);
//}

//! single name with default factories
template<typename TypeList1, typename TypeList2, typename F>
type_selector2<TypeList1, TypeList1::named::value, TypeList2, TypeList2::named::value, F> 
//type_selector2<TypeList1, true, DefaultFactory, TypeList2, true, DefaultFactory, F> 
type_selection_creator2(const std::string & type_name1, const std::string & type_name2, F & f)
{
//	static DefaultFactory df;
//
//	std::vector<std::string> names;
//	return type_selector2<TypeList1, TypeList1::named::value, DefaultFactory, TypeList2, TypeList2::named::value, DefaultFactory, F>(
//		names, df, names, df, f);
//
	std::vector<std::string> type_names1(1), type_names2(1);
	type_names1[0] = type_name1;
	type_names2[0] = type_name2;
//
//	//BOOST_MPL_ASSERT(false);
//
	return type_selection_creator2<TypeList1, TypeList2, F>(type_names1, type_names2, f);
}*/

}	// namespace type_selection

}	// namespace qfcl

#endif	// !QFCL_TYPE_SELECTION_HPP
