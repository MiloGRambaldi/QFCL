/* test/uniform_continuous.cpp
 *
 * Copyright (C) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt)
 */

#include "test_generator.ipp"
using namespace boost::unit_test_framework;

#include <qfcl/random/engine/numberline.hpp>
#include <qfcl/random/distribution/uniform_0in_1in.hpp>
#include <qfcl/random/distribution/uniform_0in_1ex.hpp>
#include <qfcl/random/distribution/uniform_0ex_1in.hpp>
#include <qfcl/random/distribution/uniform_0ex_1ex.hpp>

BOOST_AUTO_TEST_SUITE(uniform)

	BOOST_AUTO_TEST_CASE(_0in_1in)
{
	typedef qfcl::random::numberline<1,3> ENG;
	typedef qfcl::random::uniform_0in_1in<> DIST;

	BOOST_TEST_MESSAGE("Testing uniform_0in_1in with engine numberline<1, 3> ...");

	ENG eng;
	DIST dist;

	qfcl::random::variate_generator< ENG, DIST > rng(eng, dist);
	BOOST_CHECK_EQUAL( rng(), 0 );
	rng();
	BOOST_CHECK_EQUAL( rng(), 1 );
}

BOOST_AUTO_TEST_CASE(_0in_1ex)
{
	typedef qfcl::random::numberline<1,3> ENG;
	typedef qfcl::random::uniform_0in_1ex<> DIST;

	BOOST_TEST_MESSAGE("Testing uniform_0in_1ex with engine numberline<1, 3> ...");

	ENG eng;
	DIST dist;

	qfcl::random::variate_generator< ENG, DIST > rng(eng, dist);
	BOOST_CHECK_EQUAL( rng(), 0 );
	rng();
	BOOST_CHECK_LT( rng(), 1 );
}

BOOST_AUTO_TEST_CASE(_0ex_1in)
{
	typedef qfcl::random::numberline<1,3> ENG;
	typedef qfcl::random::uniform_0ex_1in<> DIST;

	BOOST_TEST_MESSAGE("Testing uniform_0ex_1in with engine numberline<1, 3> ...");

	ENG eng;
	DIST dist;

	qfcl::random::variate_generator< ENG, DIST > rng(eng, dist);
	BOOST_CHECK_GT( rng(), 0 );
	rng();
	BOOST_CHECK_EQUAL( rng(), 1 );
}

BOOST_AUTO_TEST_CASE(_0ex_1ex)
{
	typedef qfcl::random::numberline<1,3> ENG;
	typedef qfcl::random::uniform_0ex_1ex<> DIST;

	BOOST_TEST_MESSAGE("Testing uniform_0ex_1ex with engine numberline<1, 3> ...");

	ENG eng;
	DIST dist;

	qfcl::random::variate_generator< ENG, DIST > rng(eng, dist);
	BOOST_CHECK_GT( rng(), 0 );
	rng();
	BOOST_CHECK_LT( rng(), 1 );
}

BOOST_AUTO_TEST_SUITE_END()