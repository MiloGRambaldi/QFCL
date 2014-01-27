#include <algorithm>
#include <functional>
#include <vector>

#include <boost/bind.hpp>

namespace qfcl {

/// puts the square of the source in the destination
template <typename InIter, typename OutIter>
inline OutIter square_transform(InIter begin, InIter end, OutIter dest)
{
	using namespace std;

    return transform( begin, end, begin, dest, multiplies<typename InIter::value_type>() );
}

/// puts the cube of the source in the destination
template <typename InIter, typename OutIter>
inline OutIter cube_transform(InIter begin, InIter end, OutIter dest)
{
	using namespace std;
	//using namespace placeholders;

	typedef typename InIter::value_type RT;

	function<RT (RT)> cubed = boost::bind( multiplies<RT>(), _1, boost::bind( multiplies<RT>(), _1, _1 ) );

	return transform( begin, end, dest, cubed );
}

/// puts the source to the power of 4 in the destination
template <typename InIter, typename OutIter>
inline OutIter quad_transform(InIter begin, InIter end, OutIter dest)
{
	using namespace std;

	typedef typename InIter::value_type RT;

	function<RT (RT)> pow4 = boost::bind( boost::bind( multiplies<RT>(), _1, _1 ), boost::bind( multiplies<RT>(), _1, _1 ) );

	return transform( begin, end, dest, pow4 );
}

/*! Returns the set of all iterators pointing to the global maximum of the range [begin, end)
	This assumes that < || =, i.e. a < b || a == b, forms a total order.
*/
template<typename FwdIter>
inline std::vector<FwdIter>
maximum(FwdIter begin, FwdIter end)
{
	using namespace std;

	vector<FwdIter> result;

	FwdIter first_max = max_element(begin, end);

	// find all other occurences of *first_max
	for ( begin = first_max; begin != end; begin = find(begin + 1, end, *first_max) )
	{
		result.push_back(begin);
	}

	return result;
}

/*! Returns the set of all pointers to the maximal elements of the range under comp,
	which should be interpreted as less than,
	i.e. all elements x such that !comp(x,y) for all y.
	
	\note This is O(n^2), however it works for abritrary predicates comp,
	i.e. no order theoretic properties are assumed.
*/
template<typename FwdIter, typename BinaryPred>
inline std::vector<FwdIter>
maximal(FwdIter begin, FwdIter end, BinaryPred comp)
{
	using namespace std;

	vector<FwdIter> result;

	for (FwdIter outIter = begin; outIter != end; ++outIter)
	{
		result.push_back(outIter);
		for (FwdIter inIter = begin; inIter != end; ++inIter)
		{
			if ( comp(*outIter, *inIter) )
			{
				result.pop_back();
				break;
			}
		}
	}

	return result;
}

}	// namespace qfcl
