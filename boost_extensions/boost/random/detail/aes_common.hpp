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
#ifndef BOOST_RANDOM_DETAIL_AES_COMMON_HPP
#define BOOST_RANDOM_DETAIL_AES_COMMON_HPP

#include <boost/static_assert.hpp>
#include <boost/random/detail/integer_log2.hpp> // for BOOST_RANDOM_DETAIL_CONSTEXPR
#include <boost/random/detail/aes_config.hpp>
#include <boost/random/detail/prf_common.hpp>
#include <boost/random/detail/aes_hasaesni.hpp>

#if BOOST_HAS_M128i
#include <emmintrin.h>
#endif

namespace boost{
namespace random{
namespace detail{
template <typename Uint>
struct aes_common : public prf_common<128/std::numeric_limits<Uint>::digits, 128/std::numeric_limits<Uint>::digits, 128/std::numeric_limits<Uint>::digits, Uint> {
    BOOST_STATIC_ASSERT(std::numeric_limits<Uint>::is_specialized &&
                        std::numeric_limits<Uint>::is_integer &&
                        !std::numeric_limits<Uint>::is_signed &&
                        128%std::numeric_limits<Uint>::digits == 0);

    typedef prf_common<128/std::numeric_limits<Uint>::digits, 128/std::numeric_limits<Uint>::digits, 128/std::numeric_limits<Uint>::digits, Uint> common_type;
    typedef typename common_type::key_type _key_type;
    aes_common() : common_type(), useAESNI(hasAESNI())
    {}
    aes_common(_key_type _k) : common_type(_k), useAESNI(hasAESNI())
    {}

    template<class It> aes_common(It& first, It last)
        : common_type(first, last), useAESNI(hasAESNI())
    { }

    aes_common(aes_common& v) : common_type(static_cast<common_type &>(v)){}
    aes_common(const aes_common& v) : common_type(static_cast<const common_type &>(v)), useAESNI(hasAESNI())
    {}

    BOOST_RANDOM_DETAIL_SEED_SEQ_CONSTRUCTOR(aes_common, SeedSeq, seq)
        : common_type(seq), useAESNI(hasAESNI())
    {}

    BOOST_RANDOM_DETAIL_CONST_SEED_SEQ_CONSTRUCTOR(aes_common, SeedSeq, seq)
        : common_type(seq), useAESNI(hasAESNI())
    {}

    bool usehw(bool newval){
        if( newval && !hasAESNI() )
            throw std::invalid_argument("AESNI is not compiled into this binary.  aes(true) is not allowed");
        bool oldval = useAESNI;
        useAESNI = newval;
        return oldval;
    }

protected:
    bool useAESNI;
};

#if BOOST_HAS_M128i
template <>
struct aes_common<__m128i> : public prf_common<1, 1, 1, __m128i>{
    typedef prf_common<1, 1, 1, __m128i> common_type;
    typedef typename common_type::key_type _key_type;

    aes_common() : common_type(), useAESNI(hasAESNI())
    {}
    aes_common(_key_type _k) : common_type(_k), useAESNI(hasAESNI())
    {}

    template<class It> aes_common(It& first, It last)
        : common_type(first, last), useAESNI(hasAESNI())
    { }

    aes_common(aes_common& v) : common_type(static_cast<common_type &>(v)), useAESNI(v.useAESNI)
    {}
    aes_common(const aes_common& v) : common_type(static_cast<const common_type &>(v)), useAESNI(v.useAESNI)
    {}

    BOOST_RANDOM_DETAIL_SEED_SEQ_CONSTRUCTOR(aes_common, SeedSeq, seq)
        : common_type(seq), useAESNI(hasAESNI())
    {}

    BOOST_RANDOM_DETAIL_CONST_SEED_SEQ_CONSTRUCTOR(aes_common, SeedSeq, seq)
        : common_type(seq), useAESNI(hasAESNI())
    {}

    bool usehw(bool newval){
        if( newval && !hasAESNI() )
            throw std::invalid_argument("AESNI is not compiled into this binary.  aes(true) is not allowed");
        bool oldval = useAESNI;
        useAESNI = newval;
        return oldval;
    }

protected:
    bool useAESNI;
};
#endif

} // namespace detail
} // namespace random
} // namespace boost

#endif // BOOST_RANDOM_DETAIL_AES_COMMON_HPP

