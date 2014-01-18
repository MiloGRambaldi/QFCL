/* qfcl/defines.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef	QFCL_DEFINES_HPP
#define QFCL_DEFINES_HPP

/*! \file defines.hpp
	\brief Global definitions.

	\author James Hirschorn
	\date February 28, 2012
*/

#define QFCL_VERSION	0.40

//! normal sized integer
#define	QFCL_INTEGER		int
//! big integer
#define	QFCL_BIG_INTEGER	long
//! huge integer
#define	QFCL_HUGE_INTEGER	long long

/*! \defgroup macros miscellaneous macros
	@{
*/

// bad idea?
//! size of an array
#define	QFCL_ARRAY_SIZE(x)	sizeof(x) / sizeof(x[0])

//! creates and initializes a const std::vector
#define	QFCL_CREATE_VECTOR(name, type, ...)	\
	const type _##name_array[] = {__VA_ARGS__}; \
	const std::vector<type> ##name( _##name_array, _##name_array + QFCL_ARRAY_SIZE(_##name_array) )

//! creates a string literal out of a define, e.g. if #define X 0.9, then stringize(X) gives the literal "0.9".
#define QUOTE(x) #x
#define stringize(x) QUOTE(x)

//!	@}

//! \c nullptr
/* Test for GCC < 4.6.0 */
#if __GNUC__ && \
    (__GNUC__ < 4 || \
        (__GNUC__ == 4 && __GNUC_MINOR__ < 6))
const                        // this is a const object...
class {
public:
  template<class T>          // convertible to any type
    operator T*() const      // of null non-member
    { return 0; }            // pointer...
  template<class C, class T> // or any type of null
    operator T C::*() const  // member pointer...
    { return 0; }
private:
  void operator&() const;    // whose address can't be taken
} nullptr = {};              // and whose name is nullptr
#endif  // GCC < 4.6.0

//! convenience for importing types
#define QFCL_IMPORT_TYPE(T, IND_T) typedef typename IND_T::T T

//! using declaration workarounds
// apparently using typename ... defect has been fixed in GCC 4.7.0
/* Test for GCC < 4.7.0 */
#if __GNUC__ && \
    (__GNUC__ < 4 || \
        (__GNUC__ == 4 && __GNU_MINOR__ < 7))
#define __QFCL_USING_TYPE(T, IND_T) QFCL_IMPORT_TYPE(T, IND_T)
#elif _WIN32
// MSVC treats all names as dependent
// NOTE: Seems to no longer be true in VC 11.
#define __QFCL_USING_TYPE(T, IND_T) using typename IND_T::T
#else
// assume the defect is not present
#define __QFCL_USING_TYPE(T, IND_T) using typename IND_T::T
#endif

#define QFCL_USING_TYPE(T, IND_T)  __QFCL_USING_TYPE(T, IND_T)

//! non-member begin and end
// not supported by GCC until 4.6.1
/* Test for GCC < 4.6.1 */
#if __GNUC__ && \
    (__GNUC__ < 4 || \
        (__GNUC__ == 4 && \
            (__GNU_MINOR__ < 6 || \
                (__GNU_MINOR__ == 6 && __GNU_PATCHLEVEL__ < 1))))
#define __QFCL_NONMEMBER(x, name) x.name()
#else
#define __QFCL_NONMEMBER(x, name) name(x)
#endif

#define QFCL_NONMEMBER_BEGIN(x) __QFCL_NONMEMBER(x, begin)
#define QFCL_NONMEMBER_END(x)   __QFCL_NONMEMBER(x, end)

//! TMP wrapper for extracting type members
#define QFCL_MEMBER_WRAP(x) \
template<typename T>\
struct x##_wrap\
{\
    typedef typename T::x type;\
};


#endif	// !QFCL_DEFINES_HPP
