/* qfcl/utility/for_each.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef	QFCL_FOR_EACH_HPP
#define QFCL_FOR_EACH_HPP

/*! \file for_each.hpp
    \brief Meta-programming for_each, extended to cartesian products
    of sequences.

	\note Implementation note: boost::bind is used here on polymorphic
	function objects (i.e. objects with a template function call operator).
	This does not appear to be discussed in the documentation, in which case
	undocumented features of boost::bind are being used.

	\author James Hirschorn
	\date September 24, 2012
*/

//#include <functional>

#include <boost/bind.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/bind.hpp>
#include <boost/mpl/empty_sequence.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/single_view.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector.hpp>
namespace mpl = boost::mpl;

// mpl::is_lambda_expression is broken at this time
//#include <boost/tti/detail/dlambda.hpp>
//namespace tti = boost::tti::detail;

#include <qfcl/utility/for_each_fwd.hpp>
#include <qfcl/utility/tmp.hpp>

namespace qfcl {

namespace tmp {
	
namespace detail {

template<typename F>
struct for_each_object_base
{
	for_each_object_base(F f_) : f(f_) {}

	typedef void result_type;
protected:
	F f;
};
//
//template<typename F, typename T1>
//struct _mybind_object
//{
//	_mybind_object(F f, T1 t1) : f_(f), t1_(t1) {}
//
//	typedef void result_type;
//	template<typename T2>
//	result_type operator()(T2 t2) const
//	{
//		f_(t1_, t2);
//	}
//private:
//	mutable F f_;
//	T1 t1_;
//};
//
//template<typename F, typename T1>
//_mybind_object<F, T1> _mybind(F f, T1 t1)
//{
//	return _mybind_object<F, T1>(f, t1);
//}

template<typename Seq2, typename Transform2, typename F>
struct for_each_object2 : for_each_object_base<F>
{
private:
    typedef for_each_object_base<F> base_type;
    QFCL_USING_TYPE(result_type, base_type);
public:
    for_each_object2(F f) : base_type(f) {}

	template<typename T>
	result_type operator()(T t) const
	{
//		typedef mpl::eval_if<
//			boost::is_same<Transform2, IDENTITY>,
//			Seq2,
        typedef typename mpl::transform<Seq2, typename mpl::lambda< mpl::protect< mpl::bind<Transform2, T, mpl::_1> > >::type >::type Seq2_transformed;
//		>::type Seq2_transformed;

        qfcl::tmp::for_each<Seq2_transformed>(boost::bind<void>(this -> f, t, _1));
	}
};

template<typename Seq2, typename Seq3, typename F>
struct for_each_object3 : for_each_object_base<F>
{
private:
    typedef for_each_object_base<F> base_type;
    QFCL_USING_TYPE(result_type, base_type);
public:
    for_each_object3(F f) : base_type(f) {}

	template<typename T>
	result_type operator()(T t) const
	{
        qfcl::tmp::for_each<Seq2, Seq3>(boost::bind<void>(this -> f, t, _1, _2));
	}
};

template<typename Seq2, typename Seq3, typename Seq4, typename F>
struct for_each_object4 : for_each_object_base<F>
{
private:
    typedef for_each_object_base<F> base_type;
    QFCL_USING_TYPE(result_type, base_type);
public:
    for_each_object4(F f) : base_type(f) {}

	template<typename T>
	result_type operator()(T t) const
	{
        qfcl::tmp::for_each<Seq2, Seq3, Seq4>(boost::bind<void>(this -> f, t, _1, _2, _3));
	}
};

template<typename Seq2, typename Seq3, typename Seq4, typename Seq5, typename F>
struct for_each_object5 : for_each_object_base<F>
{
private:
    typedef for_each_object_base<F> base_type;
    QFCL_USING_TYPE(result_type, base_type);
public:
    for_each_object5(F f) : base_type(f) {}

	template<typename T>
	result_type operator()(T t) const
	{
        qfcl::tmp::for_each<Seq2, Seq3, Seq4, Seq5>(boost::bind<void>(this -> f, t, _1, _2, _3, _4));
	}
};

template<typename Seq2, typename Seq3, typename Seq4, typename Seq5, typename Seq6, typename F>
struct for_each_object6 : for_each_object_base<F>
{
private:
    typedef for_each_object_base<F> base_type;
    QFCL_USING_TYPE(result_type, base_type);
public:
    for_each_object6(F f) : base_type(f) {}

	template<typename T>
	result_type operator()(T t) const
	{
        qfcl::tmp::for_each<Seq2, Seq3, Seq4, Seq5, Seq6>(boost::bind<void>(this -> f, t, _1, _2, _3, _4, _5));
	}
};

#undef is_lambda_expression 

}	// namespace detail

//! IDENTITY maps <T1...TN> to TN 
struct IDENTITY
{
private:
	template<typename T1, typename T2>
	struct imp : mpl::identity<T2> {};
	template<typename T1>
	struct imp<T1, mpl::na> : mpl::identity<T1> {};
public:
	template<typename T1 = mpl::na, typename T2 = mpl::na>
	struct apply : imp<T1, T2> {};
};

struct INSTANTIATION
{
	template<typename T1, typename T2>
	struct apply : mpl::apply<T2, T1> {};
};


template<typename Sequence1, typename F>
inline 
void for_each(F f)
{
	mpl::for_each<Sequence1>(f);
}

template<typename Sequence1, typename TransformOp1, typename F>
inline 
void for_each(
	F f, 
    typename boost::enable_if< detail::is_operator<TransformOp1> >::type * dummy
)
{
	mpl::for_each<Sequence1, TransformOp1>(f);
}

template<typename Sequence1, typename Sequence2, typename F>
void for_each(
	F f, 
    typename boost::disable_if< detail::is_operator<Sequence2> >::type * dummy
)
{
	BOOST_MPL_ASSERT((mpl::is_sequence<Sequence1>));
	mpl::for_each<Sequence1>(detail::for_each_object2<Sequence2, IDENTITY, F>(f));
}

template<typename Sequence1, typename Sequence2, typename TransformOp1, typename TransformOp2, typename F>
void for_each(
	F f,
    typename boost::enable_if<detail::is_operator<TransformOp1>>::type * dummy
)
{
	mpl::for_each<Sequence1, TransformOp1>(detail::for_each_object2<Sequence2, TransformOp2, F>(f));
}

template<typename Sequence1, typename Sequence2, typename Sequence3, typename F>
void for_each(F f)
{
	BOOST_MPL_ASSERT((mpl::and_<mpl::is_sequence<Sequence1>, mpl::is_sequence<Sequence2>, mpl::is_sequence<Sequence3>>));
	mpl::for_each<Sequence1>(detail::for_each_object3<Sequence2, Sequence3, F>(f));
}

template<typename Sequence1, typename Sequence2, typename Sequence3, typename Sequence4, typename F>
void for_each(
	F f,
    typename boost::disable_if<detail::is_operator<Sequence3>>::type * dummy
)
{
	BOOST_MPL_ASSERT((mpl::and_<mpl::is_sequence<Sequence1>, mpl::is_sequence<Sequence2>, mpl::is_sequence<Sequence4>>));
	mpl::for_each<Sequence1>(detail::for_each_object4<Sequence2, Sequence3, Sequence4, F>(f));
}

template<typename Sequence1, typename Sequence2, typename Sequence3, typename Sequence4, typename Sequence5, typename F>
void for_each(F f)
{
	BOOST_MPL_ASSERT((mpl::and_<
		mpl::is_sequence<Sequence1>, 
		mpl::is_sequence<Sequence2>, 
		mpl::is_sequence<Sequence3>,
		mpl::is_sequence<Sequence4>,
		mpl::is_sequence<Sequence5>
	>));
	mpl::for_each<Sequence1>(detail::for_each_object5<Sequence2, Sequence3, Sequence4, Sequence5, F>(f));
}

// The limit is 5 by default.
#if BOOST_MPL_LIMIT_METAFUNCTION_ARITY >= 6
template<typename Sequence1, typename Sequence2, typename Sequence3, typename Sequence4, typename Sequence5, typename Sequence6, typename F>
void for_each(F f)
{
	BOOST_MPL_ASSERT((mpl::and_<
		mpl::is_sequence<Sequence1>, 
		mpl::is_sequence<Sequence2>, 
		mpl::is_sequence<Sequence3>,
		mpl::is_sequence<Sequence4>,
		mpl::is_sequence<Sequence5>,
		mpl::is_sequence<Sequence6>
	>));
	mpl::for_each<Sequence1>(detail::for_each_object6<Sequence2, Sequence3, Sequence4, Sequence5, Sequence6, F>(f));
}
#endif

}	// namespace tmp

namespace fusion
{

namespace detail
{
	
template<typename F>
struct for_each_object_base
{
	for_each_object_base(F f_) : f(f_) {}
protected:
	F f;
};

template<typename Seq2, typename F>
struct for_each_object2 : for_each_object_base<F>
{
	for_each_object2(Seq2 & seq2_, F f) 
        : for_each_object_base<F>(f), seq2(seq2_) {}

    typedef void result_type;

	template<typename T>
    result_type operator()(T t) const
	{
        qfcl::fusion::for_each(seq2, boost::bind<void>(this -> f, t, _1));
	}
protected:
	Seq2 & seq2;
};

template<typename Seq2, typename Seq3, typename F>
struct for_each_object3 : for_each_object2<Seq2, F>
{
	for_each_object3(Seq2 & seq2_, Seq3 & seq3_, F f)
		: for_each_object2<Seq2, F>(seq2_, f), seq3(seq3_) {}

    typedef void result_type;

    template<typename T>
    result_type operator()(T t) const
    {
        qfcl::fusion::for_each(this -> seq2, seq3, boost::bind<void>(this -> f, t, _1, _2));
	}
protected:
	Seq3 & seq3;
};

template<typename Seq2, typename Seq3, typename Seq4, typename F>
struct for_each_object4 : for_each_object3<Seq2, Seq3, F>
{
	for_each_object4(Seq2 & seq2_, Seq3 & seq3_, Seq4 & seq4_, F f)
		: for_each_object3<Seq2, Seq3, F>(seq2_, seq3_, f), seq4(seq4_) {}

    typedef void result_type;

    template<typename T>
    result_type operator()(T t) const
	{
        qfcl::fusion::for_each(this -> seq2, this -> seq3, seq4, boost::bind<void>(this -> f, t, _1, _2, _3));
	}
protected:
	Seq4 & seq4;
};

template<typename Seq2, typename Seq3, typename Seq4, typename Seq5, typename F>
struct for_each_object5 : for_each_object4<Seq2, Seq3, Seq4, F>
{
	for_each_object5(Seq2 & seq2_, Seq3 & seq3_, Seq4 & seq4_, Seq5 & seq5_, F f)
		: for_each_object4<Seq2, Seq3, Seq4, F>(seq2_, seq3_, seq4_, f), seq5(seq5_) {}

    typedef void result_type;

    template<typename T>
    result_type operator()(T t) const
	{
        qfcl::fusion::for_each(this -> seq2, this -> seq3, this -> seq4, seq5, boost::bind<void>(this -> f, t, _1, _2, _3, _4));
	}
protected:
	Seq5 & seq5;
};

template<typename Seq2, typename Seq3, typename Seq4, typename Seq5, typename Seq6, typename F>
struct for_each_object6 : for_each_object5<Seq2, Seq3, Seq4, Seq5, F>
{
	for_each_object6(Seq2 & seq2_, Seq3 & seq3_, Seq4 & seq4_, Seq5 & seq5_, Seq6 & seq6_, F f)
		: for_each_object5<Seq2, Seq3, Seq4, Seq5, F>(seq2_, seq3_, seq4_, seq5_, f), seq6(seq6_) {}

    typedef void result_type;

    template<typename T>
    result_type operator()(T t) const
	{
        qfcl::fusion::for_each(this -> seq2, this -> seq3, this -> seq4, this -> seq5, seq6, boost::bind<void>(this -> f, t, _1, _2, _3, _4, _5));
	}
protected:
	Seq6 & seq6;
};

}

template<typename Sequence1, typename F>
inline
void for_each(Sequence1 & seq, F f)
{
	boost::fusion::for_each(seq, f);
}

template<typename Sequence1, typename Sequence2, typename F>
void for_each(Sequence1 & seq1, Sequence2 & seq2, F f)
{
	boost::fusion::for_each(seq1, detail::for_each_object2<Sequence2, F>(seq2, f));
}

template<typename Sequence1, typename Sequence2, typename Sequence3, typename F>
void for_each(Sequence1 & seq1, Sequence2 & seq2, Sequence3 & seq3, F f)
{
	boost::fusion::for_each(seq1, detail::for_each_object3<Sequence2, Sequence3, F>(seq2, seq3, f));
}

template<typename Sequence1, typename Sequence2, typename Sequence3, typename Sequence4, typename F>
void for_each(Sequence1 & seq1, Sequence2 & seq2, Sequence3 & seq3, Sequence4 & seq4, F f)
{
	boost::fusion::for_each(seq1, detail::for_each_object4<Sequence2, Sequence3, Sequence4, F>(seq2, seq3, seq4, f));
}

template<typename Sequence1, typename Sequence2, typename Sequence3, typename Sequence4, typename Sequence5, typename F>
void for_each(Sequence1 & seq1, Sequence2 & seq2, Sequence3 & seq3, Sequence4 & seq4, Sequence5 & seq5, F f)
{
	boost::fusion::for_each(
		seq1, 
		detail::for_each_object5<Sequence2, Sequence3, Sequence4, Sequence5, F>(seq2, seq3, seq4, seq5, f)
		);
}

template<typename Sequence1, typename Sequence2, typename Sequence3, typename Sequence4, typename Sequence5, typename Sequence6, typename F>
void for_each(Sequence1 & seq1, Sequence2 & seq2, Sequence3 & seq3, Sequence4 & seq4, Sequence5 & seq5, Sequence6 & seq6, F f)
{
	boost::fusion::for_each(
		seq1, 
		detail::for_each_object6<Sequence2, Sequence3, Sequence4, Sequence5, Sequence6, F>(seq2, seq3, seq4, seq5, seq6, f)
		);
}

}

}	// namespace qfcl

#endif // !QFCL_FOR_EACH_HPP
