/* qfcl/utility/comma_separated_number.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef	QFCL_COMMA_SEPARATED_NUMBER_HPP
#define QFCL_COMMA_SEPARATED_NUMBER_HPP

/*! \file comma_separated_number.hpp
	\brief Formatting for numbers with comma separated by thousands

	\note Not implemented for floating types yet.

	\author James Hirschorn
	\date June 24, 2012
*/

#include <algorithm>
#include <iterator>
#include <string>

#include <boost/lexical_cast.hpp>

#include <qfcl/defines.hpp>

namespace qfcl {

namespace io {

template<typename T>
struct custom_formatted_type
{
	custom_formatted_type(T x) : value(x) {}

	T value;
};

template<typename T>
custom_formatted_type<T> custom_formatted(T x)
{
	return custom_formatted_type<T>(x);
}

//! custom << operator for long long that outputs a comma delimited number 
std::ostream & operator<<( std::ostream & os, 
						   custom_formatted_type<long long> ll )
{
    using namespace std;

	// the long long value
	long long i = ll.value;

	// convert to string
	string str = boost::lexical_cast<string>(i); 

	// comma separated string
	string cs_str;
	int counter = 0;

	for (string::reverse_iterator i = str.rbegin(); i != str.rend(); 
		 ++i, ++counter)
	{
		// don't put comma immediately after - sign
		if (counter == 3 && !( i + 1 == str.rend() && *i == '-' ) )
		{
			cs_str.push_back(',');
			counter = 0;
		}

		cs_str.push_back(*i);
	}
    reverse( QFCL_NONMEMBER_BEGIN(cs_str), QFCL_NONMEMBER_END(cs_str) );

	os << cs_str;

	return os;
}

// custom formatted int
std::ostream & operator<<( std::ostream & os, 
						   custom_formatted_type<int> i )
{
	return operator<<( os, custom_formatted<long long>(i.value) );
}

// custom formatted long
std::ostream & operator<<( std::ostream & os, 
						   custom_formatted_type<long> i )
{
	return operator<<( os, custom_formatted<long long>(i.value) );
}

// custom formatted unsigned int
std::ostream & operator<<( std::ostream & os,
						   custom_formatted_type<unsigned> u )
{
	return operator<<( os, custom_formatted<long long>(u.value) );
}

// custom formatted unsigned long
std::ostream & operator<<( std::ostream & os,
						   custom_formatted_type<unsigned long> u )
{
	return operator<<( os, custom_formatted<long long>(u.value) );
}

// NOTE: Fix this. It will not work for ull too large.
// custom formatted unsigned long long
std::ostream & operator<<( std::ostream & os,
						   custom_formatted_type<unsigned long long> ull )
{
	return operator<<( os, custom_formatted<long long>(ull.value) );
}

}	// namespace io

}	// namespace qfcl

#endif	// QFCL_COMMA_SEPARATED_NUMBER_HPP
