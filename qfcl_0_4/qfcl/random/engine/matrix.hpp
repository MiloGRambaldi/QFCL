/* qfcl/random/engine/matrix.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef	QFCL_RANDOM_MATRIX_HPP
#define	QFCL_RANDOM_MATRIX_HPP

/*! \file qfcl/random/engine/matrix.hpp
	\brief Vectors and matrices for linear generators

	\author James Hirschorn
	\date June 11, 2012
*/

#pragma warning(disable:4290)

#include <fstream>
#include <stdexcept>
#include <string>

#include <NTL/mat_GF2.h>

#include <qfcl/utility/io.hpp>

namespace qfcl {

namespace random {

//! identity element for type T
template<typename T>
T identity(const T &)
{
    return static_cast<T>(1);
}

template<>
NTL::mat_GF2 identity<NTL::mat_GF2>(const NTL::mat_GF2 & M);

namespace detail {

/// NOTE: Should have a proper place for this */
//! generic pow function for positive powers
template<typename T>
T pow(const T & M, unsigned long long e)
{
	if (e == 0)
		return identity(M);

	T result;
	bool initialized = false;

	T pow2 = M;

	for (;;)
	{
		if (e & 1)
		{
			result = initialized ? result * pow2 : pow2;
			initialized = true;
		}

		if (e >>= 1)
			// square pow2
			pow2 *= pow2;
		else
			break;
	}

	return result;
}

}	// namespace detail

/*! \ingroup random
	@{
*/
	
/* the vector and matrix types for a modulus m generator */

template<size_t m>
class Vector
{
};

template<size_t m>
class Matrix
{
};

template<>
class Vector<2> : public NTL::vec_GF2
{
public:
	//! default constructor
	Vector() : NTL::vec_GF2() {}
	//! implicit conversion from base types
	Vector(const NTL::vec_GF2 & v) : NTL::vec_GF2(v) {}
	//! should return an integral type
	int operator[](size_t i) const {return NTL::rep( NTL::vec_GF2::operator[](i) );}
	NTL::subscript_GF2 operator[](size_t i) {return NTL::vec_GF2::operator[](i);}
};

template<>
class Matrix<2> : public NTL::mat_GF2
{
public:
	//! default constructor
	Matrix() : NTL::mat_GF2() {}
	//! implicit conversion from base type
	Matrix(const NTL::mat_GF2 & M) : NTL::mat_GF2(M) {}

	//! write the matix to the file \c filename
    void write(const std::string & filename) const throw(std::runtime_error);
	//! read the matrix from the file \c filename, and indicate if it exists
	bool read(const std::string & filename) throw(std::runtime_error);
};

template<>
Matrix<2> identity< Matrix<2> >(const Matrix<2> & M);

//! @}

}	// namespace random

}	// namespace qfcl

#endif	// QFCL_RANDOM_MATRIX_HPP
