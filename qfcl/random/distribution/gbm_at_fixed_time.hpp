//  Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
//
//  Use, modification and distribution are subject to the BOOST Software License. 
// (See accompanying file LICENSE.txt)

#ifndef QFCL_RANDOM_DISTRIBUTION_GBM_AT_FIXED_TIME
#define QFCL_RANDOM_DISTRIBUTION_GBM_AT_FIXED_TIME

#include <cmath>
#include <qfcl/random/variate_generator.hpp>
#include <qfcl/random/distribution/normal_inversion.hpp>

namespace qfcl {
namespace random {

template<class RealType = double>
class gbm_at_fixed_time_distribution
{
private:
    // Geometric Brownian motion parameters:
    RealType m_S0;
    RealType m_vol;
    RealType m_yield;

    // Time parameters
    RealType m_t;

    // Constants for speeding up calculation
    RealType m_drift;
    RealType m_diffusion;

    // generating normal distributed random numbers
    qfcl::random_normal_distribution<RealType> nd;

public:
    typedef RealType input_type;
    typedef RealType result_type;
    
    // Constructor
    gbm_at_fixed_time(
        RealType S0,
        RealType vol,
        RealType yield,
        RealType t) 
        :
        m_S0(S0),
        m_vol(vol),
        m_yield(yield),
        m_t(t)
    {
        m_drift = (m_yield - m_r - 0.5*m_vol*m_vol)*m_t;
        m_diffusion = std::sqrt(m_t)*m_vol;
    }
    
    // Generate random number
    template<class Engine>
    result_type operator()(Engine& eng) const
    {
        qfcl::random::normal_inversion<RealType> ND;
        RealType N = ND(eng);
        
        return m_S0 * std::exp( m_drift + m_diffusion*N );
    }

};

typedef gbm_at_fixed_time_distribution<double> gbm_at_fixed_time;
}} //namespace
#endif