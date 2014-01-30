// FDMVisitor.hpp
//
// Simple class to hold finite difference schemes. In
// this version we work with doubles for convenience.
//
// (C) Datasim Education BV 2007-2011
//


#ifndef FDMVisitor_HPP
#define FDMVisitor_HPP

#include "Sde.hpp"
#include "SdeVisitor.hpp"
#include "Range.cpp"

#include <boost/mpl/string.hpp>
#include <boost/numeric/ublas/matrix.hpp>		// The matrix class.
#include <boost/numeric/ublas/io.hpp>			// Sending to IO stream.

#include <qfcl/utility/adapters.hpp>
#include <qfcl/utility/tmp.hpp>

#include "NormalGenerator.cpp"

// NOTE: Put in the appropriate place.
template<typename X>
struct pathType : boost::numeric::ublas::vector<X> 
{
	// default ctor
	pathType() {}
	// ctor initializing path of length N with init_val
	pathType(size_t N, X init_val)
		: boost::numeric::ublas::vector<X>(N, init_val) {}
};

template <typename X, typename Time, typename RT, typename Generator>
			class FdmVisitor : public SdeVisitor<X, Time, RT>
{

public: // For convenience
	
	
	// Mesh data
	boost::numeric::ublas::vector<Time> x;

	Time k;		// Time step
	Time T;

	Time sqrk;	// Square root of k (for dW stuff)
	X VOld, VNew;
	Time time;

	// Result path
	pathType<X> res;

	// Random numbers 
	BoostNormal<Generator> generator;

	// Number of steps
	long N;

	Sde<X,Time,RT> sde;

public:
	FdmVisitor() {}

	FdmVisitor(long NSteps, const Sde<X,Time,RT>& mySde, const Generator& generator);

	virtual pathType<X> & path();

};

namespace detail {
	
namespace mpl = boost::mpl;

typedef mpl::string<'E', 'x', 'p', 'l', 'i', 'c', 'i', 't'>::type Explicit_string;
typedef mpl::string<'E', 'u', 'l', 'e', 'r'>::type Euler_string;

typedef qfcl::tmp::concatenate<Explicit_string, Euler_string>::type ExplicitEuler_name;

}	// namespace detail

template <typename X, typename Time, typename RT,  typename Generator>
	class ExplicitEuler : public FdmVisitor<X,Time,RT, Generator>
{ // Explicit Euler method

private:
    typedef FdmVisitor<X, Time, RT, Generator> base_type;
	
public:
    /* inherit from base clase */
    using base_type::res;
    using base_type::x;
    using base_type::time;
    using base_type::k;
    using base_type::sqrk;
    using base_type::generator;
    using base_type::N;

	ExplicitEuler() {}
	ExplicitEuler(long NSteps, Sde<X,Time,RT>& sde, const Generator& generator);

	void Visit(Sde<X,Time,RT>& sde);
};

template <typename X, typename Time, typename RT,  typename Generator>
	class ExplicitEulerTypeII : public FdmVisitor<X,Time,RT, Generator>
{ // Explicit Euler by creating a random array beforehand

private:
    typedef FdmVisitor<X, Time, RT, Generator> base_type;

	boost::numeric::ublas::vector<Time> dW2; 

public:
    /* inherit from base clase */
    using base_type::res;
    using base_type::x;
    using base_type::time;
    using base_type::k;
    using base_type::sqrk;
    using base_type::generator;
    using base_type::N;

    ExplicitEulerTypeII() {}
	ExplicitEulerTypeII(long NSteps, Sde<X,Time,RT>& sde, const Generator& generator);

	void Visit(Sde<X,Time,RT>& sde);
};

template <typename X, typename Time, typename RT,  typename Generator>
	class ExplicitEulerMM : public FdmVisitor<X,Time,RT, Generator>
{ // Explicit Euler by Momen matching (quadratic resampling

private:
    typedef FdmVisitor<X, Time, RT, Generator> base_type;

	boost::numeric::ublas::vector<Time> dW2; 

public:
    /* inherit from base clase */
    using base_type::res;
    using base_type::x;
    using base_type::time;
    using base_type::k;
    using base_type::sqrk;
    using base_type::generator;
    using base_type::N;

    ExplicitEulerMM() {}
	ExplicitEulerMM(long NSteps, Sde<X,Time,RT>& sde, const Generator& generator);

	void Visit(Sde<X,Time,RT>& sde);
};

template <typename X, typename Time, typename RT,  typename Generator>
	class PredictorCorrector : public FdmVisitor<X,Time,RT, Generator>
{ // Adapted Predictor Corrector method

private:
    typedef FdmVisitor<X, Time, RT, Generator> base_type;

	X A, B;
	X VMid;	// Predictor value

public:
    /* inherit from base clase */
    using base_type::res;
    using base_type::x;
    using base_type::time;
    using base_type::k;
    using base_type::sqrk;
    using base_type::generator;
    using base_type::N;

    PredictorCorrector() {}
	PredictorCorrector(long NSteps, Sde<X,Time,RT>& sde, const Generator& generator,
						X alpha, X beta);

	void Visit(Sde<X,Time,RT>& sde);
};
			
template <typename X, typename Time, typename RT,  typename Generator>
	class PredictorCorrectorClassico : public FdmVisitor<X,Time,RT, Generator>
{ // Standard Predictor Corrector method: 1) no drift adjust, 2) trapezoidal averaging

private:
    typedef FdmVisitor<X, Time, RT, Generator> base_type;

	X A, B;
	X VMid;	// Predictor value

public:
    /* inherit from base clase */
    using base_type::res;
    using base_type::x;
    using base_type::time;
    using base_type::k;
    using base_type::sqrk;
    using base_type::generator;
    using base_type::N;

    PredictorCorrectorClassico() {}
	PredictorCorrectorClassico(long NSteps, Sde<X,Time,RT>& sde, const Generator& generator,
						X alpha, X beta);

	void Visit(Sde<X,Time,RT>& sde);
};

template <typename X, typename Time, typename RT,  typename Generator>
	class RichardsonEuler : public FdmVisitor<X,Time,RT, Generator>
{ // Richardson extrapolation

private:
        typedef FdmVisitor<X, Time, RT, Generator> base_type;

        // Values in one-step method
        X VOld, VNew;

        boost::numeric::ublas::vector<Time> res2;
        boost::numeric::ublas::vector<Time> x2;
        boost::numeric::ublas::vector<Time> dW2;

        double k2, sqrk2;

public:
        /* inherit from base clase */
        using base_type::res;
        using base_type::x;
        using base_type::time;
        using base_type::k;
        using base_type::sqrk;
        using base_type::generator;
        using base_type::N;

        RichardsonEuler() {}
        RichardsonEuler(long NSteps, Sde<X,Time,RT>& sde, const Generator& generator);

        void Visit(Sde<X,Time,RT>& sde);
};

template <typename X, typename Time, typename RT,  typename Generator>
	class Milstein : public FdmVisitor<X,Time,RT, Generator>
{ // Richardson extrapolation

private:
    typedef FdmVisitor<X, Time, RT, Generator> base_type;

public:
    /* inherit from base clase */
    using base_type::res;
    using base_type::x;
    using base_type::time;
    using base_type::k;
    using base_type::sqrk;
    using base_type::generator;
    using base_type::N;

    Milstein() {}
	Milstein(long NSteps, Sde<X,Time,RT>& sde, const Generator& generator);

	void Visit(Sde<X,Time,RT>& sde);
};

template <typename X, typename Time, typename RT,  typename Generator>
	class KarhunenLoeve : public FdmVisitor<X,Time,RT, Generator>
{ // Richardson extrapolation

private:
    typedef FdmVisitor<X, Time, RT, Generator> base_type;

	double tol, sqrT;
	long Ntrunc; // Computed number of needed terms
	boost::numeric::ublas::vector<Time> dW2; 

	X orthogonalFunction(Time t, long n);
	X KLExpansion(Time s, Time t);

public:
    /* inherit from base clase */
    using base_type::res;
    using base_type::x;
    using base_type::time;
    using base_type::k;
    using base_type::sqrk;
    using base_type::generator;
    using base_type::N;
    using base_type::T;

    KarhunenLoeve() {}
    KarhunenLoeve(long NSteps, Sde<X,Time,RT>& sde, const Generator& generator, double tolerance);

	void Visit(Sde<X,Time,RT>& sde);
};

// Predictor-Corrector with Karhunen-Loeve
template <typename X, typename Time, typename RT,  typename Generator>
	class PredictorCorrectorKL : public FdmVisitor<X,Time,RT, Generator>
{ // Standard Predictor Corrector method

private:
    typedef FdmVisitor<X, Time, RT, Generator> base_type;

	X A, B;
	X VMid;	// Predictor value

	double tol, sqrT;
	long Ntrunc; // Computed number of needed terms
	boost::numeric::ublas::vector<Time> dW2; 

	X orthogonalFunction(Time t, long n);
	X KLExpansion(Time s, Time t);

public:
    /* inherit from base clase */
    using base_type::res;
    using base_type::x;
    using base_type::time;
    using base_type::k;
    using base_type::sqrk;
    using base_type::generator;
    using base_type::N;
    using base_type::T;

    PredictorCorrectorKL() {}
	PredictorCorrectorKL(long NSteps, Sde<X,Time,RT>& sde, const Generator& generator,
						X alpha, X beta, double tolerance);

	void Visit(Sde<X,Time,RT>& sde);
};

#endif
