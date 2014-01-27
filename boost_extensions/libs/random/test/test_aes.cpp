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
#include <boost/random/aes.hpp>
#include <boost/cstdint.hpp>

#include "test_kat.hpp"

using boost::random::aes;

using boost::random::test::RandomNumberFunctor;
BOOST_CONCEPT_ASSERT((RandomNumberFunctor< boost::random::aes<uint32_t> >));
BOOST_CONCEPT_ASSERT((RandomNumberFunctor< boost::random::aes<uint64_t> >));

// The KAT vectors are cut-and-pasted from the kat_vectors file
// in the original Random123 distribution.  The generators that
// produce these known answers have been extensively tested and are
// known to be Crush-Resistant (see Salmon et al, "Parallel Random
// Numbers:  As Easy as 1, 2, 3")
BOOST_AUTO_TEST_CASE(test_kat_aes4x32)
{
    dokat<aes<uint32_t> >("00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000   d44be966 3b2c8aef 59fa4c88 2e2b34ca");
    dokat<aes<uint32_t> >("ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff 00000000 00000000   0f68399f cc680a67 4cbd230d 816d2e23");
    dokat<aes<uint32_t> >("243f6a88 85a308d3 13198a2e 03707344 a4093822 299f31d0 082efa98 ec4e6c89   ca693cbf 134a4f64 965e0cfd 5217a28f");
    dokat<aes<uint32_t> >("33221100 77665544 bbaa9988 ffeeddcc 03020100 07060504 0b0a0908 0f0e0d0c   d8e0c469 30047b6a 80b7cdd8 5ac5b470");
}

BOOST_AUTO_TEST_CASE(test_kat_aes2x64)
{
    dokat<aes<uint64_t> >("0000000000000000 0000000000000000 0000000000000000 0000000000000000    3b2c8aefd44be966 2e2b34ca59fa4c88");
    dokat<aes<uint64_t> >("ffffffffffffffff ffffffffffffffff ffffffffffffffff 0000000000000000   cc680a670f68399f  816d2e234cbd230d");
    dokat<aes<uint64_t> >("85a308d3243f6a88  0370734413198a2e  299f31d0a4093822  ec4e6c89082efa98    134a4f64ca693cbf  5217a28f965e0cfd");
    dokat<aes<uint64_t> >("7766554433221100 ffeeddccbbaa9988  0706050403020100  0f0e0d0c0b0a0908    30047b6ad8e0c469  5ac5b47080b7cdd8");
}
