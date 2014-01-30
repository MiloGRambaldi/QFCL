/* qfcl/statistics/descriptive.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt)
 */

/*! \file qfcl/statistics/descriptive.hpp
	\brief class providing descriptive statistics
	\note This was written quite some time ago, long before qfcl. 
	The code may be in need of redesign.

	\author James Hirschorn
	\date June 28, 2012
*/

#ifndef	QFCL_STATISTICS_DESCRIPTIVE_HPP
#define	QFCL_STATISTICS_DESCRIPTIVE_HPP

#include <functional>
#include <iomanip>
#include <map>
#include <numeric>
#include <ostream>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/foreach.hpp>

#include <qfcl/algorithm.hpp>
#include <qfcl/utility/comma_separated_number.hpp>
#include <qfcl/math/simple/functions.hpp>

#define QFCL_STATISTICS_NUMROWS	20
#define QFCL_STATISTICS_PRECISION 5

namespace qfcl {

namespace statistics {
	
// taken from libs/random/test/histogram.cpp from boost, and modified
template<typename It>
void plot_histogram(std::ostream & os, It slots_begin, It slots_end, double scale,
                    double from, double to, size_t nRows, size_t prec)
{
	long double m = *std::max_element(slots_begin, slots_end);
	// 2 - 1 for decimal and 1 for - sign
	const size_t x_label_width = static_cast<size_t>(log10(m)) + 2 + prec;

	auto store_flags = os.flags();
	auto store_prec = os.precision();

	os.setf(std::ios::fixed|std::ios::left);
	os.precision(prec);
	for (size_t r = 0; r < nRows; r++) 
	{
		long double y = static_cast<long double>(nRows - r) / nRows * m / scale; 
		os << std::setw(x_label_width) << std::right << y << "  ";
		for (It slot = slots_begin; slot != slots_end; ++slot)
		{
			char out = ' ';
			if (*slot / double(scale) >= y)
				out = 'x';
			os << out;
		}
		os << std::endl;
	}
	os.flags(store_flags);

	os << std::setw(x_label_width + 2) << " "
	   << std::setw(10) << from;
	os.setf(std::ios::right, std::ios::adjustfield);
	os << std::setw(std::distance(slots_begin, slots_end) - 10) << to << std::endl;

	os.flags(store_flags);
	os.precision(store_prec);
}

template<typename It>
void plot_log_histogram(std::ostream & os, It slots_begin, It slots_end, double scale,
						double from, double to, size_t nRows, size_t prec)
{
	long double M = *std::max_element(slots_begin, slots_end) / scale;
	long double m = *std::min_element(slots_begin, slots_end) / scale;

	// 2 - 1 for decimal and 1 for - sign
	const size_t x_label_width = static_cast<size_t>(log10(M)) + 2 + prec;

	auto store_flags = os.flags();
	auto store_prec = os.precision();

	os.setf(std::ios::fixed|std::ios::left);
	os.precision(prec);
	for (size_t r = 0; r < nRows; r++) 
	{
		//long double z = static_cast<long double>(nRows - r) / nRows;
		//long double y = pow(10, -(qfcl::math::one<long double>() - z) / z) * (M - m) + m;
		long double y = pow(10, -static_cast<long double>(r) * (prec - 1) / nRows) * (M - m) + m;
		os << std::setw(x_label_width) << std::right << y << "  ";
		for (It slot = slots_begin; slot != slots_end; ++slot)
		{
			char out = ' ';
			if (*slot / double(scale) >= y)
				out = 'x';
			os << out;
		}
		os << std::endl;
	}
	os.flags(store_flags);

	os << std::setw(x_label_width + 2) << " "
	   << std::setw(10) << from;
	os.setf(std::ios::right, std::ios::adjustfield);
	os << std::setw(std::distance(slots_begin, slots_end) - 10) << to << std::endl;

	os.flags(store_flags);
	os.precision(store_prec);
}

/** a type of Property Pattern */

template <typename ValType = double>
class Property
{
public:
	/// default ctor
	Property() : set_(false) {}
	/// ctor
	Property(const std::string & name) : name_(name), set_(false) {}

	ValType operator()() const {return value;}				/// returns the value associated with the property
	void operator()(ValType x) {value = x; set_ = true;}	/// sets the value associated with the property
	std::string name() const {return name_;}				/// returns the property name
	void name(const std::string & s) {name_ = s;}			/// sets the propery name
	bool set() const {return set_;}							/// whether the property has been set

	virtual ~Property() {}
private:
	std::string name_;
	bool set_;
	ValType value;
};

template<typename T = double>
class DescriptiveStatistics
{
	typedef std::vector<T> Vector;
	typedef std::map<T, size_t> Map;
	typedef std::map<T, T> RealMap;
	typedef std::multiset<T> MSet;
	typedef std::set<T> Set;
public:
	DescriptiveStatistics(const Vector & values);			/// sample is given as Vector
	template<typename Key, typename CounterType>
	DescriptiveStatistics(const std::map<Key, CounterType> & m); /// sample is given as a map, allow key to be converted to T
	template <typename RealIter>
	DescriptiveStatistics(RealIter begin, RealIter end);	/// sample is given as an iterator range

	size_t size() const {return n;}
	T min() const {return computeProperty(MIN);}			/// min
	T max() const {return computeProperty(MAX);}

	T mean() const {return computeProperty(MEAN);}		/// sample mean
	T median() const {return computeProperty(MEDIAN);}	/// median (roughly, the 50% quantile: middle number when odd length, avg. of 2 mid numbers when even length)
	Set mode() const;										/// mode (high frequency, when defined)
	T var() const {return computeProperty(VAR);}			/// sample variance (unbiased, cf. variance())
    Vector sample() const {return v;}                   /// returns the sample as a vector
	T sd() const {return computeProperty(SD);}			/// sample standard deviation = sqrt(var())
	T se() const {return computeProperty(SE);}			/// sample standard error = sd() / n
	T skew() const {return computeProperty(SKEW);}		/// sample Fisher Skew (cf. FisherSkew())
	T kurt() const {return computeProperty(KURT);}		/// sample kurtosis 
	T ExcessKurtosis() const {return computeProperty(EXKURT);}		/// sample excess kurtosis (cf. ExcessKurtosis())

	T CM(unsigned k) const;								/// k-th central moment
	T moment(unsigned k) const;							/// sample k-th moment
	T m2() const	{return moment(2);}						/// sample 2nd moment
	T sqrt_m2() const {return computeProperty(SQRT_M2);}	/// sqrt( sample 2nd moment )

	T EmpVar() const {return computeProperty(EMP_VAR);}	/// empirical variance: 1 / n * sum_i y_i - mean(y)
	T EmpSD() const {return computeProperty(EMP_SD);}	/// empirical standard deviation: sqrt( EmpVar() )
	T EmpSkew() const {return computeProperty(EMP_SKEW);}/// empirical Skew
	T EmpKurt() const {return computeProperty(EMP_KURT);}/// empirical Kurtosis
	T EmpExcessKurt() const {return computeProperty(EMP_EXKURT);}	/// empirical Excess Kurtosis

	T EmpMoment(unsigned k) const;						/// empirical k-th moment
	T EmpCM(unsigned k) const;							/// empirical centered k-th moment

	T Jarque_Bera() const {return computeProperty(JB);}	/// Jarque-Bera normality test, asymptotically chi-squared with 2 degrees of freedom

	Vector zscores() const;								/// zscores

	T cdf(T x) const;									/// cumulative distribution function: P[X <= x]
	T quantile(T a) const;								/// inverse cdf: x st. P[X <= x] == a
	T CondExp(T x, bool lower = true) const;			/// conditional expectation: E[X | X <= x], or X >= x if lower == false
	
	DescriptiveStatistics Tail(T a) const;				/// returns a new DescriptiveStatistics containing X <= quantile(a)

	std::ostream & ShowFrequencies(std::ostream & os) const; //, const string & label) const;
	std::ostream & histogram(std::ostream & os, size_t slots, T from, T end, 
		size_t num_rows = QFCL_STATISTICS_NUMROWS, size_t prec = QFCL_STATISTICS_PRECISION) const;
	std::ostream & distribution_histogram(std::ostream & os, size_t slots, T from, T end, 
		size_t num_rows = QFCL_STATISTICS_NUMROWS, size_t prec = QFCL_STATISTICS_PRECISION) const;
	std::ostream & log_distribution_histogram(std::ostream & os, size_t slots, T from, T end, 
		size_t num_rows = QFCL_STATISTICS_NUMROWS, size_t prec = QFCL_STATISTICS_PRECISION) const;
private:
	const Vector v;
	//mutable MSet sorted_;		/// sorted values
	//mutable Map sorted_;
	mutable Map mapped_;		/// mapped (i.e. counted) values
	mutable RealMap log_mapped_;/// log mapped
	mutable Map cdf_;			/// cdf
	mutable RealMap quantile_;	/// quantile

	void initialize(); // used by ctor
	//template <typename RealIter>
	//static Vector getVector(RealIter begin, RealIter end);
	template<typename Key, typename CounterType>
	Vector map_to_vector(const std::map<Key, CounterType> & m);
	const size_t n;			/// number of elements
	class EmptySample {};	/// Exception for attempting to construct empty DescriptiveStatistics

	struct linear_bin_type
	{
		void initialize(size_t slots, T lower_, T upper_) {lower = lower_; factor = 1 / (upper_ - lower_) * slots;}

		size_t operator()(T x) const {return static_cast<size_t>((x - lower) * factor);}
	private:
		T lower;
		T factor;
	};
	//struct logScale_bin_type
	//{
	//	void initialize(size_t slots, T lower_, T upper_)
	//};
	template<typename BinType>
	std::vector<size_t> getBins(size_t slots, T & lower, T & upper, BinType bt) const;
	//std::vector<T> getLogBins(size_t slots, T & lower, T & upper) const;

	/// statistical properties
	enum PropertyType {MIN = 0, MAX, MEAN, MEDIAN, VAR, SD, SE, SKEW, KURT, EXKURT, EMP_VAR, EMP_SD, EMP_SKEW, EMP_KURT, EMP_EXKURT, CENTERED, M2, SQRT_M2, M3, M4, 
					   EMP_M2, EMP_M3, EMP_M4, CM3, CM4, EMP_CM3, EMP_CM4, JB, MAPPED, LOG_MAPPED, CDF, QUANTILE, _END};
	static const size_t NumPropertyTypes = _END;
	static T (DescriptiveStatistics::* const mp[NumPropertyTypes])() const;
	static const std::string PropertyName[NumPropertyTypes];	/// property name is not actually used, at least for now
	mutable Property<> prop[NumPropertyTypes];
	std::function<T ()> prop_call[NumPropertyTypes];
	T computeProperty(PropertyType p) const;

	T compute_min() const;
	T compute_max() const;

	T compute_mean() const;
	T compute_centered() const;

	T compute_median() const;

	T compute_EmpM2() const;
	T compute_EmpVar() const;
	T compute_EmpSD() const;
	T compute_M2() const;
	T compute_var() const;
	T compute_sqrt_M2() const;
	T compute_sd() const;
	T compute_se() const;

	T compute_EmpM3() const;
	T compute_EmpCM3() const;
	T compute_EmpSkew() const;
	T compute_M3() const;
	T compute_CM3() const;
	T compute_skew() const;

	T compute_EmpM4() const;
	T compute_EmpCM4() const;
	T compute_EmpKurt() const;
	T compute_EmpExcKurt() const;
	T compute_M4() const;
	T compute_CM4() const;
	T compute_kurt() const;
	T compute_ExcKurt() const;

	T compute_JB() const;

	T compute_mapped() const;
	T compute_log_mapped() const;
	T compute_cdf() const;
	T compute_quantile() const;

	mutable Vector centered_;	/// centered values (i.e. demeaned)

	mutable bool computedZScores;
	mutable Vector zscores_;
	void computeZScores() const;
};

// initialize
template<typename T>
void DescriptiveStatistics<T>::initialize()
{
	if (n == 0)
		throw EmptySample();

	for (unsigned i = MEAN; i < _END; ++i)
		prop[i].name(PropertyName[i]);
}

// vector ctor
template<typename T>
DescriptiveStatistics<T>::DescriptiveStatistics(const typename DescriptiveStatistics<T>::Vector & values)
	: v(values), n(v.size()), centered_(n), zscores_(n)
{
	initialize();
}

// map_to_vector
template<typename T>
template<typename Key, typename CounterType>
typename DescriptiveStatistics<T>::Vector
DescriptiveStatistics<T>::map_to_vector(const std::map<Key, CounterType> & m)
{
	Vector v;

	typedef std::map<Key, CounterType> map_t;

    std::pair<typename map_t::key_type, typename map_t::mapped_type> p;
	BOOST_FOREACH(p, m)
	{
		v.insert( v.end(), p.second, p.first);
	}

	// mark the MAPPED as computed
	prop[MAPPED](0);

	return v;
}

// map ctor
template<typename T>
template<typename Key, typename CounterType>
DescriptiveStatistics<T>::DescriptiveStatistics(const std::map<Key, CounterType> & m)
	: v(map_to_vector(m)), n(v.size()), centered_(n), zscores_(n)
{
	initialize();
}

// template ctor
template<typename T>
template<typename RealIter>
DescriptiveStatistics<T>::DescriptiveStatistics(RealIter begin, RealIter end) 
	: v(begin, end), n( v.size() ), centered_(n), zscores_(n)
{
	initialize();
}

//template<typename T>
//template<typename RealIter>
//typename DescriptiveStatistics<T>::Vector 
//DescriptiveStatistics<T>::getVector(RealIter begin, RealIter end)
//{
//	Vector v;
//
//	throw("not currently implemented");
//	//copy( begin, end, back_inserter(v) );
//
//	return v;
//}

// computeProperty
template<typename T>
T DescriptiveStatistics<T>::computeProperty(PropertyType p) const
{
	if (!prop[p].set())
	{
		prop[p]( (this ->* mp[p])() );
	}

	return prop[p]();
}

// min
template<typename T>
T DescriptiveStatistics<T>::compute_min() const
{
	return *min_element( begin(v), end(v) );
}

// max
template<typename T>
T DescriptiveStatistics<T>::compute_max() const
{
	return *max_element( begin(v), end(v) );
}

// mean
template<typename T>
T DescriptiveStatistics<T>::compute_mean() const
{
	T sum = std::accumulate( begin(v), end(v), 0.);

	return sum / n;
}

// median
template<typename T>
T DescriptiveStatistics<T>::compute_median() const
{
	return quantile(qfcl::math::half<T>());
}

/// mode
template<typename T>
typename DescriptiveStatistics<T>::Set 
DescriptiveStatistics<T>::mode() const
{
	computeProperty(MAPPED);

	typedef typename Map::iterator Iter;
	typedef typename Map::value_type pair_type;
	std::vector<Iter> maxima = qfcl::maximal( begin(mapped_), end(mapped_), 
		[] (pair_type x, pair_type y) {return x.second < y.second;} );

	Set mode;
	std::for_each(begin(maxima), end(maxima), [&] (Iter p) {mode.insert(p -> first);});

	return mode;
}

/// centered
template<typename T>
T DescriptiveStatistics<T>::compute_centered() const
{
	using namespace std;

	transform( begin(v), end(v), begin(centered_), boost::bind( minus<T>(), _1, mean() ) );

	return 0.;
}

// mapped
template<typename T>
T DescriptiveStatistics<T>::compute_mapped() const
{
	BOOST_FOREACH(T x, v)
		mapped_[x] = mapped_.count(x) != 0 ? mapped_[x] + 1 : 1; 

	return 0.;
}

// mapped
template<typename T>
T DescriptiveStatistics<T>::compute_log_mapped() const
{
	computeProperty(MAPPED);

	BOOST_FOREACH(T x, v)
		log_mapped_[x] = log( static_cast<T>(mapped_[x]) ); 

	return 0.;
}

// compute_cdf
template<typename T>
T DescriptiveStatistics<T>::compute_cdf() const
{
	computeProperty(MAPPED);

	size_t accumulator = 0;

	typedef typename Map::value_type pair_type;
	BOOST_FOREACH(pair_type p, mapped_)
	{
		accumulator += p.second;
		cdf_[p.first] = accumulator;
	}

	return 0.;
}

// compute_quantile
template<typename T>
T DescriptiveStatistics<T>::compute_quantile() const
{
	using namespace std;

	computeProperty(CDF);

    for_each( begin(cdf_), end(cdf_), [=] (typename Map::value_type p) {quantile_[p.first] = T(p.second) / n;});

	return 0.;
}

/// Empirical Var
template<typename T>
T DescriptiveStatistics<T>::compute_EmpVar() const
{
	/// computed centered values
	computeProperty(CENTERED);

	Vector temp(n);
	square_transform( begin(centered_), end(centered_), begin(temp) );
	
	T sum = accumulate( begin(temp), end(temp), 0.);

	return sum / n;
}

template<typename T>
T DescriptiveStatistics<T>::compute_EmpM2() const
{
	Vector temp(n);
	square_transform( begin(v), end(v), begin(temp) );

	T sum = accumulate(temp.begin(), temp.end(), 0.);
	
	return sum / n;
}

/// var
template<typename T>
T DescriptiveStatistics<T>::compute_var() const
{
	T rn(n);

	return EmpVar() * rn / (rn - 1);
}

/// M2
template<typename T>
T DescriptiveStatistics<T>::compute_M2() const
{
	T rn(n);

	T emp_m2( computeProperty(EMP_M2) );

	return emp_m2 * rn / (rn - 1);
}

/// Empirical SD
template<typename T>
T DescriptiveStatistics<T>::compute_EmpSD() const
{
	return sqrt( EmpVar() );
}

/// sqrt( M2 )
template<typename T>
T DescriptiveStatistics<T>::compute_sqrt_M2() const
{
	return sqrt( m2() );
}

/// SD
template<typename T>
T DescriptiveStatistics<T>::compute_sd() const
{
	return sqrt( var() );
}

/// SE
template<typename T>
T DescriptiveStatistics<T>::compute_se() const
{
	return sd() / sqrt(static_cast<T>(n));
}

/// Empirical central 3rd moment
template<typename T>
T DescriptiveStatistics<T>::compute_EmpCM3() const
{
	computeProperty(CENTERED);

	Vector temp(n);
	cube_transform( begin(centered_), end(centered_), begin(temp) );

	T sum = accumulate( begin(temp), end(temp), 0. );

	return sum / n;
}

/// Emprical 3rd moment
template<typename T>
T DescriptiveStatistics<T>::compute_EmpM3() const
{
	Vector temp(n);
	cube_transform( begin(v), end(v), begin(temp) );

	T sum = accumulate( begin(temp), end(temp), 0. );

	return sum / n;
}

/// Empirical Skew
template<typename T>
T DescriptiveStatistics<T>::compute_EmpSkew() const
{
	return computeProperty(EMP_CM3) / pow(EmpSD(), 3);
}

/// 3rd central moment
template<typename T>
T DescriptiveStatistics<T>::compute_CM3() const
{
	T rn(n);

	return (computeProperty(EMP_CM3) * rn) * rn / ( (rn - 1) * (rn - 2) );
}

/// 3rd moment
template<typename T>
T DescriptiveStatistics<T>::compute_M3() const
{
	T rn(n);

	return (computeProperty(EMP_M3) * rn) * rn / ( (rn - 1) * (rn - 2) );
}

/// skew 
template<typename T>
T DescriptiveStatistics<T>::compute_skew() const
{
	return computeProperty(CM3) / pow(sd(), 3);
}

/// Emprical 4th moment
template<typename T>
T DescriptiveStatistics<T>::compute_EmpM4() const
{
	Vector temp(n);
	quad_transform( begin(v), end(v), begin(temp) );

	T sum = accumulate( begin(temp), end(temp), 0. );

	return sum / n;
}

/// Empirical central 4th moment
template<typename T>
T DescriptiveStatistics<T>::compute_EmpCM4() const
{
	computeProperty(CENTERED);

	Vector temp(n);
	quad_transform( begin(centered_), end(centered_), temp.begin() );

	T sum = accumulate( begin(temp), end(temp), 0. );

	return sum / n;
}

/// Empirical Kurtosis
template<typename T>
T DescriptiveStatistics<T>::compute_EmpKurt() const
{
	return computeProperty(EMP_CM4) / pow(EmpSD(), 4);
}

/// Empirical Excess Kurtosis
template<typename T>
T DescriptiveStatistics<T>::compute_EmpExcKurt() const
{
	return EmpKurt() - 3.;
}

/// 4th moment
template<typename T>
T DescriptiveStatistics<T>::compute_M4() const
{
	T rn(n);

	return (computeProperty(EMP_M4) * rn) * ( rn * (rn + 1) ) / ( (rn - 1) * (rn - 2) * (rn - 3) );
}

/// 4th central moment
template<typename T>
T DescriptiveStatistics<T>::compute_CM4() const
{
	T rn(n);

	return (computeProperty(EMP_CM4) * rn) * ( rn * (rn + 1) ) / ( (rn - 1) * (rn - 2) * (rn - 3) );
}

/// sample kurtosis
template<typename T>
T DescriptiveStatistics<T>::compute_kurt() const
{
	return computeProperty(CM4) / pow( sd(), 4);
}

/// sample excess kurtosis
template<typename T>
T DescriptiveStatistics<T>::compute_ExcKurt() const
{
	return kurt() - ( 3. * ( (n - 1) * (n - 1) ) / ( (n - 2) * (n - 3) ) );
}

/// sample k-th moment
template<typename T>
T DescriptiveStatistics<T>::moment(unsigned k) const
{
	if (k > 4)
	throw("Error: moment not implemented for k > 4");
		
	switch(k)
	{
		case 0:
			return 1.;
		case 1:
			return mean();
		case 2:
			return computeProperty(M2);
		case 3:
			return computeProperty(M3);
		case 4:
			return computeProperty(M4);
		default:
			throw("Should never be here!");
	}
}

/// sample k-th central moment
template<typename T>
T DescriptiveStatistics<T>::CM(unsigned k) const
{
	if (k > 4)
		throw("Error: CM not implemented for k > 4");
		
	switch(k)
	{
		case 0:
			return 1.;
		case 1:
			return mean();
		case 2:
			return var();
		case 3:
			return computeProperty(CM3);
		case 4:
			return computeProperty(CM4);
		default:	
			throw("Should never be here!");
	}
}

/// Empirical k-th moment
template<typename T>
T DescriptiveStatistics<T>::EmpMoment(unsigned k) const
{
    using namespace std;
    using std::placeholders::_1;

	switch( k )
	{
	case 0:
		return 1.;
	case 1:
		return mean();
	case 2:
		return computeProperty(EMP_M2);
	case 3:
		return computeProperty(EMP_M3);
	case 4:
		return computeProperty(EMP_M4);
	default:	/// k > 4
		Vector temp(n);
        transform( v.begin(), v.end(), temp.begin(), bind( pow<T>, _1, k ) );

        auto sum = accumulate(temp.begin(), temp.end(), 0.);
		return sum / n;
	}
}

/// Empirical central k-th moment
template<typename T>
T DescriptiveStatistics<T>::EmpCM(unsigned k) const
{
    using namespace std;
    using std::placeholders::_1;

	switch( k )
	{
	case 0:
		return 1.;
	case 1:
		return mean();
	case 2:
		return computeProperty(EMP_VAR);
	case 3:
		return computeProperty(EMP_CM3);
	case 4:
		return computeProperty(EMP_CM4);
	default:	/// k > 4
		computeProperty(CENTERED);

		Vector temp(n);
        transform( centered_.begin(), centered_.end(), temp.begin(), bind( pow<T>, _1, k ) );

		T sum = accumulate(temp.begin(), temp.end(), 0.);
		return sum / n;
	}
}

/// JB
template<typename T>
T DescriptiveStatistics<T>::compute_JB() const
{
	T rn(n);

	return (rn / 6) * ( pow(EmpSkew(), 2) + pow(EmpExcessKurt(), 2) / 4 );
}

/// computeZScores
template<typename T>
void DescriptiveStatistics<T>::computeZScores() const
{
	if (!computedZScores)
	{
        transform( v.begin(), v.end(), zscores_.begin(), boost::bind( std::divides<T>(), boost::bind( std::minus<T>(), _1, mean() ), sd() ) );

		computedZScores = true;
	}
}

// cdf
template<typename T>
T DescriptiveStatistics<T>::cdf(T x) const
{
	computeProperty(CDF);

    typename Map::const_iterator upper = cdf_.upper_bound(x);

	if (upper == cdf_.begin())
		return T(0);
	else
		return T((--upper) -> second) / n;
}

// quantile
template<typename T>
T DescriptiveStatistics<T>::quantile(T a) const
{
	using namespace std;

	// domain error
	if (a < T(0) || a > T(1))
		throw domain_error("quantile must be inbetween 0 and 1");

	computeProperty(QUANTILE);

    auto q = find_if(begin(quantile_), end(quantile_), [=] (typename RealMap::value_type p) {return p.second >= a;});
	auto r = q;

	// use averaging for ties
	if ( ++r != end(quantile_) && qfcl::math::approx_equal(q -> second, a) )
	{
		return (r -> first + q -> first) / 2;
	}
	else
		return q -> first;
}

// Tail
template<typename T>
DescriptiveStatistics<T> DescriptiveStatistics<T>::Tail(T a) const
{
	T q = quantile(a);

	return DescriptiveStatistics( Map( begin(mapped_), mapped_.upper_bound(q) ) );
}

template<typename T>
std::ostream & DescriptiveStatistics<T>::ShowFrequencies(std::ostream & os) const
{
	using namespace std;

	computeProperty(MAPPED);

//	os << "Frequencies of occurences for " 
//		 << qfcl::io::custom_formatted(n) << " trials:\n";

    for_each(begin(mapped_), end(mapped_), [&] (typename Map::value_type p)
	{
		os << "Value: " <<  p.first << " has " << p.second << " occurences.\n";
	});

	return os << endl;
}

template<typename T>
template<typename BinType>
std::vector<size_t> DescriptiveStatistics<T>::getBins(size_t slots, T & lower, T & upper, BinType bt) const
{
	using namespace std;

	computeProperty(MAPPED);

	// fix lower and upper
	lower = std::max(lower, begin(mapped_) -> first);
	auto endp = --end(mapped_);
	// adding one epsilon does not work
	upper = std::min( upper, (1 + qfcl::math::epsilon<T>()) * endp -> first );
	//assert(T(1) < 1 + qfcl::math::epsilon<T>());
	assert(endp -> first < upper);

	// initialize bt
	bt.initialize(slots, lower, upper);

	vector<size_t> bin(slots, 0);

    for_each(mapped_.lower_bound(lower), mapped_.upper_bound(upper), [=, &bin] (typename Map::value_type p)
	{
		if (p.first >= lower && p.first < upper)
			bin[bt(p.first)] += p.second; //static_cast<size_t>( (p.first - lower) / (upper - lower) * slots )] += p.second;
	});

	// debug:
	//cerr << "Debug: " << endl;
	//for (size_t i = 0; i < slots; ++i)
	//	cerr << i << ": " << bin[i] << endl;
	//cerr << endl;

	return bin;
}

template<typename T>
std::ostream & DescriptiveStatistics<T>::histogram(std::ostream & os, size_t slots, T lower, T upper, size_t nRows, size_t prec) const
{
	using namespace std;

	vector<size_t> bins = getBins(slots, lower, upper, linear_bin_type());

	plot_histogram(os, begin(bins), end(bins), 1, lower, upper, nRows, prec);
	os << endl;
	
	return os;
}

template<typename T>
std::ostream & DescriptiveStatistics<T>::distribution_histogram(std::ostream & os, size_t slots, T lower, T upper, size_t nRows, size_t prec) const
{
	using namespace std;

	vector<size_t> bins = getBins(slots, lower, upper, linear_bin_type());

	plot_histogram(os, begin(bins), end(bins), n, lower, upper, nRows, prec);
	os << endl;
	
	return os;
}

template<typename T>
std::ostream & DescriptiveStatistics<T>::log_distribution_histogram(std::ostream & os, size_t slots, T lower, T upper, 
																	size_t nRows, size_t prec) const
{
	using namespace std;

	vector<size_t> bins = getBins(slots, lower, upper, linear_bin_type());

	plot_log_histogram(os, begin(bins), end(bins), n, lower, upper, nRows, prec);
	os << endl;

	return os;
}
//
//template<typename T>
//std::ostream & DescriptiveStatistics<T>::log_distribution_histogram(std::ostream & os, size_t slots, T lower, T upper, 
//																	size_t nRows) const
//{
//	using namespace std;
//
//	vector<size_t> bins = getBins(slots, lower, upper, linear_bin_type());
//
//	// compute the precision
//	long double M = *std::max_element(begin(bins), end(bins)) / long double(n);
//	long double m = *std::min_element(begin(bins), end(bins)) / long double(n);
//	long double z = qfcl::math::one<long double>() / nRows;
//	long double y = pow(10, -(qfcl::math::one<long double>() - z) / z) * (M - m) + m;
//
//	size_t prec = static_cast<size_t>(-log10(y)) + 1;
//
//	plot_log_histogram(os, begin(bins), end(bins), n, lower, upper, nRows, prec);
//	os << endl;
//
//	return os;
//}

/// static associations for PropertyType:
/// MIN, MAX, MEAN, MEDIAN, VAR, SD, SE, SKEW, KURT, EXKURT, EMP_VAR, EMP_SD, EMP_SKEW, EMP_KURT, EMP_EXKURT, CENTERED, M2, SQRT_M2, M3, M4, 
/// EMP_M2, EMP_M3, EMP_M4, CM3, CM4, EMP_CM3, EMP_CM4, JB, MAPPED, LOG_MAPPED, CDF, QUANTILE

template<typename T>
T (DescriptiveStatistics<T>::* const DescriptiveStatistics<T>::mp[])() const = 
{
    &DescriptiveStatistics<T>::compute_min,
    &DescriptiveStatistics<T>::compute_max,
    &DescriptiveStatistics<T>::compute_mean,
    &DescriptiveStatistics<T>::compute_median,
    &DescriptiveStatistics<T>::compute_var,
    &DescriptiveStatistics<T>::compute_sd,
    &DescriptiveStatistics<T>::compute_se,
    &DescriptiveStatistics<T>::compute_skew,
    &DescriptiveStatistics<T>::compute_kurt,
    &DescriptiveStatistics<T>::compute_ExcKurt,
    &DescriptiveStatistics<T>::compute_EmpVar,
    &DescriptiveStatistics<T>::compute_EmpSD,
    &DescriptiveStatistics<T>::compute_EmpSkew,
    &DescriptiveStatistics<T>::compute_EmpKurt,
    &DescriptiveStatistics<T>::compute_EmpExcKurt,
    &DescriptiveStatistics<T>::compute_centered,
    &DescriptiveStatistics<T>::compute_M2,
    &DescriptiveStatistics<T>::compute_sqrt_M2,
    &DescriptiveStatistics<T>::compute_M3,
    &DescriptiveStatistics<T>::compute_M4,
    &DescriptiveStatistics<T>::compute_EmpM2,
    &DescriptiveStatistics<T>::compute_EmpM3,
    &DescriptiveStatistics<T>::compute_EmpM4,
    &DescriptiveStatistics<T>::compute_CM3,
    &DescriptiveStatistics<T>::compute_CM4,
    &DescriptiveStatistics<T>::compute_EmpCM3,
    &DescriptiveStatistics<T>::compute_EmpCM4,
    &DescriptiveStatistics<T>::compute_JB,
    &DescriptiveStatistics<T>::compute_mapped,
    &DescriptiveStatistics<T>::compute_log_mapped,
    &DescriptiveStatistics<T>::compute_cdf,
    &DescriptiveStatistics<T>::compute_quantile
};

// NOTE: bad ?
template<typename T>
const std::string DescriptiveStatistics<T>::PropertyName[] = 
{"mininum",				"maximum",						"sample mean",	"median",					"sample var",					"sample standard deviation",	"sample standard error", "sample skew",				"sample kurtosis",			"sample excess kurtosis",	"empirical var",	"empirical standard deviation",		"empirical skew",
 "empirical kurtosis",	"empirical excess kurtosis",	"centered",	"sample second moment",			"square root sample 2nd moment","sample third moment",			"sample fourth moment", "empirical second moment",	"empirical third moment",	"empirical fourth moment",
 "sample central third moment",		"sample central fourth moment", "emprical centered third moment", "empirical centered fourth moment",	"Jarqe-Bera test",		"mapped",				"log mapped",	"cdf",						"quantile"};

}	// namespace statistics

}	// namespace qfcl

#endif	// QFCL_STATISTICS_DESCRIPTIVE_HPP
