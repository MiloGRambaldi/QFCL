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
#include <boost/random/ars.hpp>
#include <boost/cstdint.hpp>

#include "test_kat.hpp"

using boost::random::ars;

using boost::random::test::RandomNumberFunctor;
BOOST_CONCEPT_ASSERT((RandomNumberFunctor< boost::random::ars<uint32_t> >));
BOOST_CONCEPT_ASSERT((RandomNumberFunctor< boost::random::ars<uint64_t> >));

// The KAT vectors are cut-and-pasted from the kat_vectors file
// in the original Random123 distribution.  The generators that
// produce these known answers have been extensively tested and are
// known to be Crush-Resistant (see Salmon et al, "Parallel Random
// Numbers:  As Easy as 1, 2, 3")
BOOST_AUTO_TEST_CASE(test_kat_ars4x32)
{
    dokat<ars<uint32_t, 10> >("00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000   8d73ee19 506401ef 13c2dbe4 0cbe9c0d");
    dokat<ars<uint32_t, 10> >("243f6a88 85a308d3 13198a2e 03707344 a4093822 299f31d0 082efa98 ec4e6c89   a516e7d6 8357ad74 5b59b3ec 8763fff3");
    dokat<ars<uint32_t, 10> >("ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff 00000000 00000000   bb3743b1 9f635551 ecbc87fc a19478a9");
}

BOOST_AUTO_TEST_CASE(test_kat_ars2x64)
{
    dokat<ars<uint64_t, 10> >("0000000000000000 0000000000000000 0000000000000000 0000000000000000   506401ef8d73ee19 0cbe9c0d13c2dbe4");
    dokat<ars<uint64_t, 10> >("85a308d3243f6a88  0370734413198a2e  299f31d0a4093822  ec4e6c89082efa98    8357ad74a516e7d6  8763fff35b59b3ec");
    dokat<ars<uint64_t, 10> >("ffffffffffffffff ffffffffffffffff ffffffffffffffff 0000000000000000   9f635551bb3743b1  a19478a9ecbc87fc");
}
