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
#ifndef BOOST_RANDOM_ARS_HPP
#define BOOST_RANDOM_ARS_HPP

#include <boost/array.hpp>
#include <boost/static_assert.hpp>
#include <boost/cstdint.hpp>
#include <boost/limits.hpp>
#include <cstring>
#include "detail/aes_config.hpp"
#include "detail/aes_impl.hpp"
#include "detail/aes_common.hpp"
#include "detail/aes_hasaesni.hpp"

namespace boost{
namespace random{
struct ars_constants{
    static const uint64_t W0 = UINT64_C(0x9E3779B97F4A7C15); /* golden ratio */
    static const uint64_t W1 = UINT64_C(0xBB67AE8584CAA73B); /* sqrt(3) - 1.0 */
};

template <typename Uint, unsigned R=7, typename CONSTANTS=ars_constants >
struct ars : public detail::aes_common<Uint>{
    BOOST_STATIC_ASSERT(R>0);
    typedef detail::aes_common<Uint> common_type;
    typedef typename common_type::domain_type _domain_type;
    typedef typename common_type::range_type _range_type;
    typedef typename common_type::key_type _key_type;

    ars() : common_type(){ 
    }
    ars(_key_type _k) : common_type(_k){ 
    }

    template<class It> 
    ars(It& first, It last) : common_type(first, last) {
    }

    ars(ars& v) : common_type(static_cast<common_type &>(v)) {
    }
    ars(const ars& v) : common_type(static_cast<const common_type &>(v)){
    }

    BOOST_RANDOM_DETAIL_SEED_SEQ_CONSTRUCTOR(ars, SeedSeq, seq): common_type(seq){
    }

    BOOST_RANDOM_DETAIL_CONST_SEED_SEQ_CONSTRUCTOR(ars, SeedSeq, seq): common_type(seq){
    }

    _range_type operator()(_domain_type c){ 
#if BOOST_HAS_AESNI
        if( this->useAESNI ){
            detail::hw128 c128; c128 = c;
            detail::hw128 k128; k128 = this->k;
            return static_cast<_range_type>(apply(c128, k128));
        }else
#endif
        {
            detail::sw128 c128; c128 = c;
            detail::sw128 k128; k128 = this->k;
            return static_cast<_range_type>(apply(c128, k128));
        }
    }

protected:
#if BOOST_HAS_AESNI
    detail::hw128&
    apply(detail::hw128& c128, detail::hw128 k128){
        detail::hw128 w128; w128 = std::make_pair(CONSTANTS::W0, CONSTANTS::W1);
        c128 ^= k128;
        unsigned r=R-1;
        while(r--){
            k128 += w128;
            aesenc(c128, k128);
        }
        k128 += w128;
        return aesenclast(c128, k128);
    }
#endif

    detail::sw128&
    apply(detail::sw128& c128, detail::sw128 k128);
};

// Don't inline this!
template <typename Uint, unsigned R, typename CONSTANTS>
detail::sw128&
ars<Uint, R, CONSTANTS>::apply(detail::sw128& c128, detail::sw128 k128){
    detail::sw128 w128; w128 =std::make_pair(CONSTANTS::W0, CONSTANTS::W1);
    c128 ^= k128;
    unsigned r=R-1;
    while(r--){
        k128 += w128;
        aesenc(c128, k128);
    }
    k128 += w128;
    return aesenclast(c128, k128);
}

} // namespace random
} // namespace boost

#endif // BOOST_RANDOM_ARS_HPP
