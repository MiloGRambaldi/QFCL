//  Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
//
//  Use, modification and distribution are subject to the BOOST Software License. 
// (See accompanying file LICENSE.txt)

#ifndef QFCL_GBM_VANILLA_PUT_DISTRIBUTION
#define QFCL_GBM_VANILLA_PUT_DISTRIBUTION

#include <cmath>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>


template<class RealType = double>
class gbm_vanilla_put_distribution
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

    // generating normal distributed random numbers
    boost::normal_distribution<RealType> nd;

public:
    typedef RealType input_type;
    typedef RealType result_type;
    
    // Constructor
    gbm_vanilla_put_distribution(
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
        boost::normal_distribution<RealType> ND;        
        RealType N = ND(eng);
        
        RealType St = m_S0 * std::exp( m_drift + m_diffusion*N );
        return std::max( m_npv_strike - St, 0.);
    }

};


#endif