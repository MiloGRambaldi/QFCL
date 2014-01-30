//  Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
//
//  Use, modification and distribution are subject to the BOOST Software License. 
// (See accompanying file LICENSE.txt)

#ifndef QFCL_RANDOM_DISTRIBUTION_GBM_VANILLA_CALL
#define QFCL_RANDOM_DISTRIBUTION_GBM_VANILLA_CALL

#include <cmath>
#include <qfcl/random/variate_generator.hpp>
#include <qfcl/random/distribution/normal_inversion.hpp>

namespace qfcl {

namespace random {

template< typename RealType = double, typename normal_distribution_type = normal_inversion_distribution<RealType> >
class gbm_vanilla_call_distribution
{
private:
    // Geometric Brownian motion parameters:
    RealType m_S0;
    RealType m_vol;
    RealType m_yield;
    
    // Simple interest rate for discounting future payoff
    RealType m_r;
    
    // Call option parameters
    RealType m_strike;
    RealType m_t;

    // Constants for speeding up calculation
    RealType m_drift;
    RealType m_diffusion;
    RealType m_npv_strike;



public:
    typedef RealType input_type;
    typedef RealType result_type;
    
    // Constructor
    gbm_vanilla_call_distribution(
        RealType S0,
        RealType vol,
        RealType yield,
        RealType r,
        RealType strike,
        RealType t) 
        :
        m_S0(S0),
        m_vol(vol),
        m_yield(yield),
        m_r(r),
        m_strike(strike),
        m_t(t)
    {
        m_drift = (m_yield - m_r - 0.5*m_vol*m_vol)*m_t;
        m_diffusion = std::sqrt(m_t)*m_vol;
        m_npv_strike = std::exp(-m_r*m_t)*m_strike;
    }
    
    // Generate random number
    template<class Engine>
    result_type operator()(Engine& eng) const
    {
        variate_generator<Engine, normal_distribution_type> NGen( eng, normal_distribution_type() );        
        //RealType N = ND(eng);
		RealType N = NGen();
        
        RealType St = m_S0 * std::exp( m_drift + m_diffusion*N );
        return std::max( St - m_npv_strike, 0.);
    }

};

typedef gbm_vanilla_call_distribution<double> gbm_vanilla_call;

}	// namespace random

}	// namespace qfcl

#endif