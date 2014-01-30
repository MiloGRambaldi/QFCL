/** @page LICENSE
Copyright 2010-2012, D. E. Shaw Research.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright
  notice, this list of conditions, and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions, and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

* Neither the name of D. E. Shaw Research nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef BOOST_RANDOM_DETAIL_MULHILO_HPP
#define BOOST_RANDOM_DETAIL_MULHILO_HPP

#include <limits>
#include <boost/cstdint.hpp>
#include <boost/integer.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/static_assert.hpp>

namespace boost{
namespace random{
namespace detail{

// First, we implement hilo multiplication with "half words".  This is
// the "reference implementation" which should be correct for any
// binary unsigned integral UINT with an even number of bits.  In
// practice, we try to avoid this implementation because it is so slow
// (4 multiplies plus about a dozen xor, or, +, shift, mask and
// compare operations.
template <typename Uint>
inline Uint 
mulhilo_hw(Uint a, Uint b, Uint& hip){ 
    BOOST_STATIC_ASSERT(std::numeric_limits<Uint>::is_specialized &&
                        std::numeric_limits<Uint>::is_integer &&
                        !std::numeric_limits<Uint>::is_signed &&
                        std::numeric_limits<Uint>::radix == 2 &&
                        std::numeric_limits<Uint>::digits%2 == 0);
    const unsigned WHALF = std::numeric_limits<Uint>::digits/2;
    const Uint LOMASK = ((Uint)(~(Uint)0)) >> WHALF;
    Uint lo = a*b;
    Uint ahi = a>>WHALF;
    Uint alo = a& LOMASK;
    Uint bhi = b>>WHALF;
    Uint blo = b& LOMASK;
                                                                   
    Uint ahbl = ahi*blo;
    Uint albh = alo*bhi;
                                                                   
    Uint ahbl_albh = ((ahbl&LOMASK) + (albh&LOMASK));
    Uint hi = (ahi*bhi) + (ahbl>>WHALF) +  (albh>>WHALF);
    hi += ahbl_albh >> WHALF;
    /* carry from the sum with alo*blo */                               
    hi += ((lo >> WHALF) < (ahbl_albh&LOMASK));
    hip = hi;
    return lo;
}

// We can formulate a much faster implementation if we can use
// integers of twice the width of Uint (e.g., DblUint).  Such types
// are not always available (e.g., when Uint is uintmax_t), but when
// they are, we find that modern compilers (gcc, MSVC, Intel) pattern
// match the structure of this code and turn it into an optimized
// instruction sequence, e.g., mulw or mull.

// However, the alternative implementation, which we want to use
// when there *is* a DblUint would be ambiguoous without some enable_if
// hackery.  To support that, we need a has_double_width type trait.
//
// N.B.  You might think that uint_t<2*W>::least would be just as
// correct, and maybe a little faster here.  Not so!  In boost 1.47
// and 1.48 (and maybe others), using a uint_t<128>::type is not an
// SFINAE-inducing error.  In fact, it's a 'signed char'.  A bug
// report and patch were posted on Nov 23, 2011:
// https://svn.boost.org/trac/boost/ticket/6169 
// Hopefully, the patch will be accepted before 1.49.  For now, the
// ::exact typedefs follows a completely different code path and
// tickle SFINAE as documented (and as we require).
template <typename Uint>
class has_double_width{
    typedef char yes[1];
    typedef char no[2];

    template <typename T>
    static yes& test(typename uint_t<2*std::numeric_limits<T>::digits>::exact*);
    template <typename>
    static no& test(...);
public:
    static const bool value = sizeof(test<Uint>(0)) == sizeof(yes);
};

// mulhilo using double-width DblUint
template <typename Uint>
inline typename boost::enable_if_c<has_double_width<Uint>::value, Uint>::type
mulhilo(Uint a, Uint b, Uint& hip){
    BOOST_STATIC_ASSERT(std::numeric_limits<Uint>::is_specialized &&
                        std::numeric_limits<Uint>::is_integer &&
                        !std::numeric_limits<Uint>::is_signed &&
                        std::numeric_limits<Uint>::radix == 2);
    typedef typename uint_t<2*std::numeric_limits<Uint>::digits>::exact DblUint;
    DblUint product = ((DblUint)a)*((DblUint)b);
    hip = product>>std::numeric_limits<Uint>::digits;
    return (Uint)product;
}

// When there's no DblUint, fall back to half-word implementation.
// FIXME:  Add more implementations that use asm and/or compiler intrinsics
template <typename Uint>
inline typename boost::enable_if_c<!has_double_width<Uint>::value, Uint>::type 
mulhilo(Uint a, Uint b, Uint& hip){
    return mulhilo_hw(a, b, hip);
}

#if defined(__GNUC__) && defined(__x86_64__)
template <>
inline uint64_t 
mulhilo(uint64_t ax, uint64_t b, uint64_t& hip){
    uint64_t dx;
    __asm__("\n\t"
        "mulq %2\n\t"
        : "=a"(ax), "=d"(dx)
        : "r"(b), "0"(ax)
        );
    hip = dx;
    return ax;
}
#endif

} // namespace detail
} // namespace random
} // namespace boost

#endif // BOOST_RANDOM_DETAIL_MULHILO_HPP
