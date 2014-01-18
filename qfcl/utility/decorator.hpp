/* qfcl/utility/decorator.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef	QFCL_UTILITY_DECORATOR_HPP
#define QFCL_UTILITY_DECORATOR_HPP

/*! \file decorator.hpp
    \brief Provides helpers for the decorator pattern with static polymorphism.

    \author James Hirschorn
    \date April 22, 2013
*/

#include <utility>

#include <boost/mpl/bool.hpp>
#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/if.hpp>

namespace qfcl {
namespace tmp {

namespace mpl = boost::mpl;

template<typename Derived>
struct decorator
{
BOOST_MPL_HAS_XXX_TRAIT_DEF(is_decorator)

template<typename T>
struct get_derived_type
//    : T::derived_type
{
    typedef typename T::derived_type type;
};

//template<typename Derived>
struct transitive_derived
        : mpl::eval_if<
            has_is_decorator<Derived>,
            //mpl::identity<
            get_derived_type<Derived>,
            //>,
            mpl::identity<Derived>
        >
{};

//template<typename Derived, typename BaseDecorator>
template<typename BaseDecorator>
struct base_type
        : mpl::eval_if<
            has_is_decorator<Derived>,
            mpl::identity<Derived>,
            mpl::identity<BaseDecorator>
        >
{
//protected:
//    typedef mpl::true_ is_decorator;
};
};

template<typename Base>
struct decorator_base_type
        : public Base
{
    decorator_base_type()
    {}
    template<typename T1>
    decorator_base_type(T1&& arg1) : Base(std::forward<T1>(arg1))
    {}
protected:
    // the actual typedef does not matter, the existence of is_decorator is
    typedef mpl::true_ is_decorator;
    // could be dangerous to make copies
    decorator_base_type(decorator_base_type const&);
};

}}  // namespace qfcl::tmp

#endif // !QFCL_UTILITY_DECORATOR_HPP
