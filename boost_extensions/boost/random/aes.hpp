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
#ifndef BOOST_RANDOM_AES_HPP
#define BOOST_RANDOM_AES_HPP

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

template <typename Uint>
struct aes : public detail::aes_common<Uint>{
    typedef detail::aes_common<Uint> common_type;
    typedef typename common_type::domain_type _domain_type;
    typedef typename common_type::range_type _range_type;
    typedef typename common_type::key_type _key_type;

    aes() : common_type(){ 
        xkeyinit();
    }
    aes(_key_type _k) : common_type(_k){ 
        xkeyinit();
    }

    template<class It> 
    aes(It& first, It last) : common_type(first, last) {
        xkeyinit();
    }

    aes(aes& v) : common_type(static_cast<common_type &>(v)) {
        xkeycopy(v);
    }
    aes(const aes& v) : common_type(static_cast<const common_type &>(v)){
        xkeycopy(v);
    }

    BOOST_RANDOM_DETAIL_SEED_SEQ_CONSTRUCTOR(aes, SeedSeq, seq): common_type(seq){
        xkeyinit();
    }

    BOOST_RANDOM_DETAIL_CONST_SEED_SEQ_CONSTRUCTOR(aes, SeedSeq, seq): common_type(seq){
        xkeyinit();
    }

    _range_type operator()(_domain_type c){ 
#if BOOST_HAS_AESNI
        if( this->useAESNI ){
            detail::hw128 c128; c128 = c;
            return static_cast<_range_type>(apply(c128));
        }else
#endif
        {
            detail::sw128 c128; c128 = c;
            return static_cast<_range_type>(apply(c128));
        }
    }

protected:
#if BOOST_HAS_AESNI    
    detail::hw128&
    apply(detail::hw128& c128){
        c128 ^= xkey.hw[0];
        for(int r=1; r<10; ++r)
            aesenc(c128, xkey.hw[r]);
        return aesenclast(c128, xkey.hw[10]);
    }
#endif

    detail::sw128&
    apply(detail::sw128& c128){
        c128 ^= xkey.sw[0];
        for(int r=1; r<10; ++r)
            aesenc(c128, xkey.sw[r]);
        return aesenclast(c128, xkey.sw[10]);
    }

protected:

    union{
#if BOOST_HAS_AESNI
        detail::hw128 hw[11];
#endif
        detail::sw128 sw[11];
    }xkey;

    void xkeyinit(){
#if BOOST_HAS_AESNI
        if(this->useAESNI){
            detail::hw128 hwk; hwk = this->k;
            xkeyinit(&xkey.hw[0], hwk);
        }else
#endif
        {
            detail::sw128 swk; swk  = this->k;
            xkeyinit(&xkey.sw[0], swk);
        }
    }

    void xkeycopy(const aes& other){
        for(int i=0; i<11; ++i)
            xkey.sw[i] = other.xkey.sw[i];
    }

    template <int RCON, typename T128>
    T128& keyround(T128& rkey){
        using namespace boost::detail;
        T128 tmp2 = rkey;
        detail::aeskeygenassist<RCON>(tmp2);
        detail::shuffle<0xff>(tmp2);
        T128 temp3 = rkey;
        detail::slli<0x4>(temp3);
        rkey ^= temp3;
        detail::slli<0x4>(temp3);
        rkey ^= temp3;
        detail::slli<0x4>(temp3);
        rkey ^= temp3;
        rkey ^= tmp2;
        
        return rkey;
    }

    template <typename T128>
    void xkeyinit(T128 *xkey, T128 k128){
        xkey[0] = k128;
        xkey[1] = keyround<0x1>(k128);
        xkey[2] = keyround<0x2>(k128);
        xkey[3] = keyround<0x4>(k128);
        xkey[4] = keyround<0x8>(k128);
        xkey[5] = keyround<0x10>(k128);
        xkey[6] = keyround<0x20>(k128);
        xkey[7] = keyround<0x40>(k128);
        xkey[8] = keyround<0x80>(k128);
        xkey[9] = keyround<0x1b>(k128);
        xkey[10] = keyround<0x36>(k128);
    }

};

} // namespace random
} // namespace boost

#endif // BOOST_RANDOM_AES_HPP
