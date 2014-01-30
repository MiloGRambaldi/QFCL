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
#ifndef BOOST_RANDOM_DETAIL_HASAESNI_HPP
#define BOOST_RANDOM_DETAIL_HASAESNI_HPP

#ifdef BOOST_HAS_AESNI
#if defined(_MSC_FULL_VER)
#include <intrin.h>  // for __cpuid
#endif
#endif

namespace boost{
namespace random{
namespace detail{

#ifdef BOOST_HAS_AESNI
// Note that if  BOOST_HAS_AESNI is defined, we can safely assume that
// we're on an x86_64 platform which significantly reduces the
// breadth of the #ifdefs here.
#if defined(__GNUC__) 
// FIXME - we're assuming here that the compiler groks gcc-style asm.
// Obviously, gcc does so, but so do others, e.g., Intel's icc.  Are
// there others?
// FIXME - this fails with -fPIC -m32.  It can be fixed by pushing
// ebx, or by guaranteeing that BOOST_HAS_AESNI is never true when
// compiling 32-bit.
inline bool hasAESNI(){
    unsigned int eax, ebx, ecx, edx;
    __asm__ __volatile__ ("cpuid": "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx) :
                      "a" (1));
    return (ecx>>25) & 1;
}
#elif defined(_MSC_FULL_VER)
inline bool hasAESNI(){
    int CPUInfo[4];
    __cpuid(CPUInfo, 1);
    return (CPUInfo[2]>>25)&1;
}
#else
#error "Don't know how to implement hasAESNI on this platform"
#endif // __GNUC__ or _MSC_FULL_VER

#else // BOOST_HAS_AESNI
inline bool hasAESNI(){
    return false;
}
#endif // BOOST_HAS_AESNI

} // namespace detail
} // namespace random
} // namespace boost

#endif // BOOST_RANDOM_DETAIL_HASAESNI_HPP
