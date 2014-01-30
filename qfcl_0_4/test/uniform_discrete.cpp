/* test/uniform_discrete.cpp
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

BOOST_AUTO_TEST_SUITE(numberline)

BOOST_AUTO_TEST_CASE(test_dice)
{
    qfcl::random::numberline<1,6> dice;

	BOOST_TEST_MESSAGE("Testing the 'numberline' generator with range " << dice.min() << " to " << dice.max() << " ...");
    
	BOOST_CHECK_EQUAL( dice(), 1 );
	BOOST_CHECK_EQUAL( dice(), 2 );
	BOOST_CHECK_EQUAL( dice(), 3 );
	BOOST_CHECK_EQUAL( dice(), 4 );
	BOOST_CHECK_EQUAL( dice(), 5 );
	BOOST_CHECK_EQUAL( dice(), 6 );

	BOOST_CHECK_EQUAL( dice(), 1 );
	BOOST_CHECK_EQUAL( dice(), 2 );
	BOOST_CHECK_EQUAL( dice(), 3 );
	BOOST_CHECK_EQUAL( dice(), 4 );
	BOOST_CHECK_EQUAL( dice(), 5 );
	BOOST_CHECK_EQUAL( dice(), 6 );
}

BOOST_AUTO_TEST_SUITE_END()