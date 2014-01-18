/* qfcl/math/bits/copy_bits.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef	QFCL_MATH_BITS_COPY_BITS_HPP
#define	QFCL_MATH_BITS_COPY_BITS_HPP

/*! \file qfcl/math/bits/copy_bits.hpp
	\brief copy with significant bits and shifts

	\author James Hirschorn
	\date Created April 29, 2012
*/

#include <algorithm>
#include <utility>

#include <boost/integer.hpp>

namespace qfcl {

namespace math {

// forward delcaration
template<size_t inSize, typename InIter>
struct bit_pseudoiterator_helper;

template<size_t intSize_, typename Iter>
class bit_pseudoiterator
{
    typedef bit_pseudoiterator_helper<intSize_, Iter> helper_t;
public:
	typedef typename helper_t::UIntType UIntType;
    static const size_t intSize = intSize_;

	bit_pseudoiterator(Iter _iter, size_t _shift) 
		: iter_(_iter), shift_(_shift) {}

	//IntType operator*() const {return *ptr >> shift;}
	helper_t operator*() {return helper_t(*this);}
	//helper_t operator->() {return helper_t(*this);}
	// following requires a change in design
	//helper_t * operator->() {return}

	bit_pseudoiterator & operator+=(ssize_t k);
	bit_pseudoiterator & operator-=(ssize_t k) {return *this += -k;}
	ssize_t operator-(const bit_pseudoiterator & other) const
	{
		return iter_ - other.iter_ + shift_ - other.shift_;
	}

	size_t shift() const {return shift_;}
	void shift(size_t a) 
	{
		if (a == intSize)
		{
			++iter_;
			shift_ = 0;
		}
		else
			shift_ = a;
	}
	Iter iter() const {return iter_;}

	bool operator==(bit_pseudoiterator other) const
	{
		return iter_ == other.iter_ && shift_ == other.shift_;
	}
	bool operator!=(bit_pseudoiterator other) const {return !(*this==other);}

	bool operator<(bit_pseudoiterator other) const
	{
		return iter_ < other.iter_ || (iter_ == other.iter_ && shift_ < other.shift_);
	}
	//	
	//template<typename InIntType, size_t inSize, typename InIter>
	//friend struct bit_pseudoiterator_helper;
private:
	Iter iter_;
	//! invariant: <tt>shift_ < intSize</tt>
	size_t shift_;

//	helper_t helper;
	//typedef boost::low_bits_mask_t<intSize> lmask_t;
	//static const typename lmask_t::fast lmask = lmask_t::sig_bits_fast;
	//typedef typename boost::uint_t<intSize>::fast OutType;
};

template<size_t intSize, typename Iter>
inline bit_pseudoiterator<intSize, Iter> &
bit_pseudoiterator<intSize, Iter>::operator+=(ssize_t k)
{
	const size_t newShift = shift_ + k;

	// 2 main cases are first, and have a fast implementation
	if (newShift < intSize && newShift >= 0)
	{
		shift_ = newShift;
	}
	else if (newShift == intSize)
	{
		shift_ = 0;
		++iter_;
	}
	else if (newShift > 0)
	{
		iter_ += newShift / intSize;
		shift_ = newShift % intSize;
	}
	else // newShift < 0
	{
		iter_ += (newShift + 1) / intSize - 1;
		shift_ = newShift % intSize;
	}

	return *this;
};

template<size_t intSize, typename Iter>
struct bit_pseudoiterator_helper
{
	typedef typename boost::low_bits_mask_t<intSize> lmask_t;
	static const typename lmask_t::fast lmask = lmask_t::sig_bits_fast;
	typedef typename boost::uint_t<intSize>::fast UIntType;

	//! constructor
	explicit bit_pseudoiterator_helper(bit_pseudoiterator<intSize, Iter> & iter)
		: caller(iter) {}
	/*bit_pseudoiterator_helper & operator=(bit_pseudoiterator<IntType, intSize, Iter> & iter)
	{
		caller = iter;
	}*/

	/*! \brief <tt>*dest = *src</tt>

		\note \param bit_pseudoiterator_helper is not really \c const. 
		The \c const is cast away. It is there so that the other \c operator= 
		does not get precedence.
	*/
	template<size_t inSize, typename InIter>
	bit_pseudoiterator_helper & operator=(const bit_pseudoiterator_helper<inSize, InIter> & in)
	{
		size_t nbits = std::min( intSize - caller.shift(), inSize - in.caller.shift() );
		write( *in.caller.iter() >> in.caller.shift(), nbits );

		// update in
		const_cast< bit_pseudoiterator_helper<inSize, InIter> & >(in).caller += nbits;

		return *this;
	}
	/*! <tt>operator=(IntType)</tt>
	*/
	template<typename InIntType>
	UIntType operator=(const InIntType & x)
	{
		//// clear if first write
		//if ( caller.shift() == 0 )
		//	*caller.iter() = 0;
		//else if ( caller.shift() < intSize )
		//{
		//	UIntType y = static_cast<UIntType>(x);
		//	*caller.iter() = ( ( y << caller.shift() ) & lmask ) 
		//		| ( *caller.iter() & ( ( UIntType(1) << caller.shift() ) - 1) );
		//}
		write(x, sizeof(InIntType) * 8);

		return *caller.iter();
	}
	template<typename InIntType>
	size_t write(InIntType val, size_t nbits)
	{
		size_t result;

		if (nbits > 0)
		{
			size_t newShift = std::min( caller.shift() + nbits, intSize );
			UIntType x = static_cast<UIntType>(val) << caller.shift();

			*caller.iter() = x & ( lmask >> (intSize - newShift) )
				| ( *caller.iter() & ( ( UIntType(1) << caller.shift() ) - 1) );

			result = newShift - caller.shift();
			caller.shift(newShift);
		}
		else // if nbits == 0 then the above *caller.iter() can be out of range without an error
			result = 0;

		return result;
	}
	operator UIntType() {return *caller.iter() >> caller.shift();}

	//bit_pseudoiterator_helper operator->() {return *this;}

	bit_pseudoiterator<intSize, Iter> & caller;
};

/* factory function */
template<size_t intSize, typename Iter>
bit_pseudoiterator<intSize, Iter> 
make_bit_pseudoiterator(Iter iter, size_t shift)
{
	return bit_pseudoiterator<intSize, Iter>(iter, shift);
}

/** copy bits function */

template<typename InBitIter, typename OutBitIter>
inline OutBitIter
copy_bits(InBitIter begin, InBitIter end, OutBitIter dest, bool clear = false, bool overwrite = false)
{
	// clear the low bits of dest if requested
	if (clear && begin != end)
	{
		*make_bit_pseudoiterator<OutBitIter::intSize>( dest.iter(), 0 ) = 0;
	}

	//InBitIter stop = overwrite ? end 
	//	: make_bit_pseudoiterator<InBitIter::intSize>( end.iter(), 0 );

	typename OutBitIter::UIntType last;
	while(begin < end)
	{
		last = *dest;
		*dest = *begin;
	}

	if (!overwrite)
	{
		// go back to the actual end of the target
		dest -= begin - end;
		// rewrite what was overwritten, and go back again
		dest -= (*dest).write(last, begin - end);
	}

	return dest;
}

template<typename BitIter, typename IntType>
inline void
write_lowbits(BitIter dest, IntType val)
{
	( *make_bit_pseudoiterator<BitIter::intSize>( dest.iter(), 0 ) ).write( val, dest.shift() );
}

}	// namespace math

}	// namespace qfcl

#endif	// QFCL_MATH_BITS_COPY_BITS_HPP
