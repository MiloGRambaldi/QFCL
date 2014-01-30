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
#ifndef BOOST_RANDOM_DETAIL_AES_IMPL_HPP
#define BOOST_RANDOM_DETAIL_AES_IMPL_HPP

#include <boost/endian/conversion.hpp>
#include <boost/cstdint.hpp>
#include "aes_config.hpp"

#ifdef BOOST_HAS_AESNI
#include <wmmintrin.h>
// We use _mm_testz_si128, an SSE4.2-ism, found in smmintrin.h
//
// Intel's wmmintrin includes nmmintrin(SSE4.2) which includes smmintrin(SSE4.1), 
//    cascading to tmmintrin(SSSE3) pmmintrin(SSE3) and then emmintrin(SSE2)
// GCC's wmmintrin includes emmintrin(SSE2) directly, skipping n, s, t, and p
//    so we have to ask for smmintrin explicitly.
// What about other compilers??
//
// Another option might be to #include <x86intrin.h>.  But x86intrin.h
// is gcc-specific.  MSVC calls the analogous file <intrin.h>.  Intel
// calls it <ia32intrin.h>.  
//
// The simplest solution seems to be to #include <smmintrin.h> when
// __GNUC__ is defined.  It has the additional advantage that it seems
// to work even for certain non-GNU compilers that insist on defining
// __GNUC__ "for compatibility".
#ifdef __GNUC__
#include <smmintrin.h>
#endif // __GNUC__
#else // BOOST_HAS_AESNI
// Even if we don't have aesni, we might have m128i.
#ifdef BOOST_HAS_M128i
#include <emmintrin.h>
#endif
#endif // BOOST_HAS_AESNI

namespace boost{
namespace random{
namespace detail{

// N.B.  arrays of detail::hw128 and detail::sw128 are members of a union in
// the generic AES implementation, so they may not have construtors.
// We give them templated operator= and templated operatorT()
// conversion operators in lieu of constructors.  hw128 is only
// declared if BOOST_HAS_AESNI is defined.  sw128 is always declared.

#ifdef BOOST_HAS_AESNI

#if defined(__ICC) || (defined(_MSC_VER) && !defined(_WIN64))
/* Is there an intrinsic to assemble an __m128i from two 64-bit words? 
   If not, use the 4x32-bit intrisic instead.  */
inline __m128i _mm_set_epi64x(uint64_t v1, uint64_t v0){
    union{
        uint64_t u64;
        uint32_t u32[2];
    } u1, u0;
    u1.u64 = v1;
    u0.u64 = v0;
    return _mm_set_epi32(u1.u32[1], u1.u32[0], u0.u32[1], u0.u32[0]);
}
#endif

struct hw128{ 
    __m128i v; 

    operator array<uint64_t, 2>() const{
        union{
            array<uint64_t, 2> a;
            __m128i m;
        }u;
        u.m = v;
        return u.a;
    }

    operator array<uint32_t, 4>() const{
        union{
            array<uint32_t, 4> a;
            __m128i m;
        }u;
        u.m = v;
        return u.a;
    }

    operator array<__m128i, 1>() const{
        array<__m128i, 1> ret;
        ret[0] = v;
        return ret;
    }

    hw128& operator=(array<__m128i, 1>& a){
        v = a[0];
        return *this;
    }
    hw128& operator=(const std::pair<uint64_t, uint64_t>& p){
        v = _mm_set_epi64x(p.second, p.first);
        return *this;
    }
    hw128& operator=(const array<uint64_t, 2>& a){
        union{
            array<uint64_t, 2> a;
            __m128i m;
        }u;
        u.a = a;
        v = u.m;
        return *this;
    }
    hw128& operator=(const array<uint32_t, 4>& a){
        union{
            array<uint32_t, 4> a;
            __m128i m;
        }u;
        u.a = a;
        v = u.m;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const hw128& v){
        union{
            array<uint32_t, 4> a;
            __m128i m;
        }u;
        u.m = v.v;
        return os << u.a[0] << " " << u.a[1] << " " << u.a[2] << " " << u.a[3];
    }
};

inline hw128& operator+=(hw128& lhs, hw128 rhs){
    lhs.v = _mm_add_epi64(lhs.v, rhs.v);
    return lhs;
};

inline hw128& operator^=(hw128& lhs, hw128 rhs){
    lhs.v = _mm_xor_si128(lhs.v, rhs.v);
    return lhs;
};

inline bool operator==(hw128 lhs, hw128 rhs){
    __m128i x = _mm_xor_si128(lhs.v, rhs.v);
    return _mm_testz_si128(x, x);
}

inline hw128& aesenc(hw128& lhs, hw128 rhs){
    lhs.v = _mm_aesenc_si128(lhs.v, rhs.v);
    return lhs;
}

inline hw128& aesenclast(hw128& lhs, hw128 rhs){
    lhs.v = _mm_aesenclast_si128(lhs.v, rhs.v);
    return lhs;
}

template <int Rcon>
inline hw128& aeskeygenassist(hw128& rhs){
    rhs.v = _mm_aeskeygenassist_si128(rhs.v, Rcon);
    return rhs;
}

template <int Imm>
inline hw128& slli(hw128& rhs){
    rhs.v = _mm_slli_si128(rhs.v, Imm);
    return rhs;
}

template <int Imm>
inline hw128& shuffle(hw128& rhs){
    rhs.v = _mm_shuffle_epi32(rhs.v, Imm);
    return rhs;
}

#endif // BOOST_HAS_AESNI

struct sw128 {
    typedef union{
        array<uint64_t, 2> a64;
        array<uint32_t, 4> a32;
        array<uint8_t, 16> a8;
    } u_t;
    u_t v;

    template <typename T>
    operator T() const{
        T ret;
        memcpy(ret.data(), &v, sizeof(v));
        return ret;
    }

    operator array<uint64_t, 2>() const{
        using namespace ::boost::endian;
        array<uint64_t, 2> ret;
        little_to_native(v.a64[0], ret[0]);
        little_to_native(v.a64[1], ret[1]);
        return ret;
    }

    operator array<uint32_t, 4>() const{
        using namespace ::boost::endian;
        array<uint32_t, 4> ret;
        little_to_native(v.a32[0], ret[0]);
        little_to_native(v.a32[1], ret[1]);
        little_to_native(v.a32[2], ret[2]);
        little_to_native(v.a32[3], ret[3]);
        return ret;
    }

    sw128& operator=(const std::pair<uint64_t, uint64_t>& p){
        using namespace ::boost::endian;
        native_to_little(p.first, v.a64[0]);
        native_to_little(p.second, v.a64[1]);
        return *this;
    }
#ifdef BOOST_HAS_M128i
    sw128& operator=(const array<__m128i, 1>& a){
        memcpy(&v, a.data(), sizeof(v));
        return *this;
    }

    operator array<__m128i, 1>() const{
        array<__m128i, 1> ret;
        memcpy(ret.data(), &v, sizeof(v));
        return ret;
    }
#endif
    sw128& operator=(const array<uint64_t, 2>& a){
        using namespace ::boost::endian;
        native_to_little(a[0], v.a64[0]);
        native_to_little(a[1], v.a64[1]);
        return *this;
    }
    sw128& operator=(const array<uint32_t, 4>& a){
        using namespace ::boost::endian;
        native_to_little(a[0], v.a32[0]);
        native_to_little(a[1], v.a32[1]);
        native_to_little(a[2], v.a32[2]);
        native_to_little(a[3], v.a32[3]);
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& os, const sw128& v){
        return os << v.v.a32[0] << " " << v.v.a32[1] << " " << v.v.a32[2] << " " << v.v.a32[3];
    }
};

inline sw128& operator+=(sw128& lhs, sw128 rhs){
    lhs.v.a64[0] += rhs.v.a64[0];
    lhs.v.a64[1] += rhs.v.a64[1];
    return lhs;
};

inline sw128& operator^=(sw128& lhs, sw128 rhs){
    lhs.v.a64[0] ^= rhs.v.a64[0];
    lhs.v.a64[1] ^= rhs.v.a64[1];
    return lhs;
};

inline bool operator==(sw128 lhs, sw128 rhs){
    return lhs.v.a64 == rhs.v.a64;
}

// A lookup table for the Rijndael Sbox is pretty much unavoidable.
uint8_t sbox[256] = 
{
   0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
   0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
   0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
   0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
   0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
   0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
   0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
   0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
   0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
   0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
   0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
   0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
   0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
   0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
   0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
   0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

inline uint32_t subword(uint32_t x){
    sw128::u_t u;
    u.a32[0] = x;
    for(int i=0; i<4; ++i)
        u.a8[i] = sbox[u.a8[i]];
    return u.a32[0];
}

inline sw128& subbytes(sw128& lhs){
    for(int i=0; i<16; ++i)
        lhs.v.a8[i] = sbox[lhs.v.a8[i]];
    return lhs;
}    

inline sw128& shiftrows(sw128& lhs){
    uint8_t tmp = lhs.v.a8[1];
    lhs.v.a8[1] = lhs.v.a8[5];
    lhs.v.a8[5] = lhs.v.a8[9];
    lhs.v.a8[9] = lhs.v.a8[13];
    lhs.v.a8[13] = tmp;

    std::swap(lhs.v.a8[2], lhs.v.a8[10]);
    std::swap(lhs.v.a8[6], lhs.v.a8[14]);

    tmp = lhs.v.a8[15];
    lhs.v.a8[15] = lhs.v.a8[11];
    lhs.v.a8[11] = lhs.v.a8[7];
    lhs.v.a8[7] = lhs.v.a8[3];
    lhs.v.a8[3] = tmp;
    return lhs;
}

// A little faster than shiftrows followed by subbytes
inline sw128& shiftrows_subbytes(sw128& lhs){
    lhs.v.a8[0] = sbox[lhs.v.a8[0]];
    lhs.v.a8[4] = sbox[lhs.v.a8[4]];
    lhs.v.a8[8] = sbox[lhs.v.a8[8]];
    lhs.v.a8[12] = sbox[lhs.v.a8[12]];

    uint8_t tmp = sbox[lhs.v.a8[1]];
    lhs.v.a8[1] = sbox[lhs.v.a8[5]];
    lhs.v.a8[5] = sbox[lhs.v.a8[9]];
    lhs.v.a8[9] = sbox[lhs.v.a8[13]];
    lhs.v.a8[13] = tmp;

    tmp = sbox[lhs.v.a8[2]];
    lhs.v.a8[2] = sbox[lhs.v.a8[10]];
    lhs.v.a8[10] = tmp;

    tmp = sbox[lhs.v.a8[6]];
    lhs.v.a8[6] = sbox[lhs.v.a8[14]];
    lhs.v.a8[14] = tmp;

    tmp = sbox[lhs.v.a8[15]];
    lhs.v.a8[15] = sbox[lhs.v.a8[11]];
    lhs.v.a8[11] = sbox[lhs.v.a8[7]];
    lhs.v.a8[7] = sbox[lhs.v.a8[3]];
    lhs.v.a8[3] = tmp;
    return lhs;
}

inline void mixcolumn(uint8_t *cp){
    // The mixcolumns matrix is:
    //  [ 2 3 1 1 ]
    //  [ 1 2 3 1 ]
    //  [ 1 1 2 3 ]
    //  [ 3 1 1 2 ]
    // Populate vectors c1 and c2 so that:
    // 1*in[i] = c1[i]
    // 2*in[i] = c2[i]
    // 3*in[i] = c2[i]^c1[i]
    uint8_t c1[4];
    uint8_t c2[4];
    for(int i=0; i<4; ++i){
        c1[i] = cp[i];
        c2[i] = cp[i]<<1;
        if(c1[i]&0x80)
            c2[i] ^= 0x1b;
    }
    cp[0] = c2[0] ^ c1[3] ^ c1[2] ^ c2[1] ^ c1[1];
    cp[1] = c2[1] ^ c1[0] ^ c1[3] ^ c2[2] ^ c1[2];
    cp[2] = c2[2] ^ c1[1] ^ c1[0] ^ c2[3] ^ c1[3];
    cp[3] = c2[3] ^ c1[2] ^ c1[1] ^ c2[0] ^ c1[0];
}

inline sw128& mixcolumns(sw128& lhs){
#if 0
    mixcolumn(&lhs.v.a8[0]);
    mixcolumn(&lhs.v.a8[4]);
    mixcolumn(&lhs.v.a8[8]);
    mixcolumn(&lhs.v.a8[12]);
#else
    sw128::u_t c1 = lhs.v;
    sw128::u_t c2;
    sw128::u_t c3;
    uint64_t hibit;
    uint64_t oneb;
    // c2 is an array of 2*lhs.v, in the byte-oriented
    // algebra used by mixcolumns.
    // First pick out the high bit of each byte.
    hibit = c1.a64[0] & UINT64_C(0x8080808080808080);
    // Then compute oneb=(hibit)?0x1b:0;
    oneb = (hibit>>7) * 0x1b;
    // Finally, c2 = oneb ^ (c1<<1)
    c2.a64[0] = oneb^ ((c1.a64[0]<<1) & UINT64_C(0xfefefefefefefefe));
    hibit = c1.a64[1] & UINT64_C(0x8080808080808080);
    oneb = (hibit>>7) * 0x1b;
    c2.a64[1] = oneb^ ((c1.a64[1]<<1) & UINT64_C(0xfefefefefefefefe));
    // c3 is an array of 3*lhs.v in the mixcolumns algebra.
    c3.a64[0] = c1.a64[0]^c2.a64[0];
    c3.a64[1] = c1.a64[1]^c2.a64[1];
    
    // Now populate each 4-byte "column" with a linear
    // operator in the mixcolumns algebra.  The matrix is:
    //  [ 2 3 1 1 ]
    //  [ 1 2 3 1 ]
    //  [ 1 1 2 3 ]
    //  [ 3 1 1 2 ]
    for(size_t off=0; off<16; off += 4){
        lhs.v.a8[off+0] = c2.a8[off+0]^c3.a8[off+1]^c1.a8[off+2]^c1.a8[off+3];
        lhs.v.a8[off+1] = c1.a8[off+0]^c2.a8[off+1]^c3.a8[off+2]^c1.a8[off+3];
        lhs.v.a8[off+2] = c1.a8[off+0]^c1.a8[off+1]^c2.a8[off+2]^c3.a8[off+3];
        lhs.v.a8[off+3] = c3.a8[off+0]^c1.a8[off+1]^c1.a8[off+2]^c2.a8[off+3];
    }
#endif
    return lhs;
}

inline uint32_t rotword(uint32_t x){
    return (x<<24) | (x>>8);
}

sw128& aesenc(sw128& lhs, sw128 rhs){
    shiftrows_subbytes(lhs);
    mixcolumns(lhs);
    lhs ^= rhs;
    return lhs;
}

sw128& aesenclast(sw128& lhs, sw128 rhs){
    shiftrows_subbytes(lhs);
    lhs ^= rhs;
    return lhs;
}

template <int Rcon>
sw128& aeskeygenassist(sw128& rhs){
    rhs.v.a32[0] = subword(rhs.v.a32[1]);
    rhs.v.a32[1] = rotword(rhs.v.a32[0]) ^ Rcon;
    rhs.v.a32[2] = subword(rhs.v.a32[3]);
    rhs.v.a32[3] = rotword(rhs.v.a32[2]) ^ Rcon;
    return rhs;
}

template <int Imm>
inline sw128& slli(sw128& rhs){
    BOOST_STATIC_ASSERT(Imm==0x4);
    rhs.v.a32[3] = rhs.v.a32[2];
    rhs.v.a32[2] = rhs.v.a32[1];
    rhs.v.a32[1] = rhs.v.a32[0];
    rhs.v.a32[0] = 0;
    return rhs;
}

template <int Imm>
inline sw128& shuffle(sw128& rhs){
    BOOST_STATIC_ASSERT(Imm==0xff);
    rhs.v.a32[2] = rhs.v.a32[3];
    rhs.v.a32[1] = rhs.v.a32[3];
    rhs.v.a32[0] = rhs.v.a32[3];
    return rhs;
}

} // namespace detail
} // namespace random
} // namespace boost

#endif // BOOST_RANDOM_DETAIL_AES_IMPL_HPP
