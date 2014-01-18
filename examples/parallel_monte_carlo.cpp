#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/random/counter_based_urng.hpp>
#include <boost/random/philox.hpp>

#include <boost/random/variate_generator.hpp>

#include <qfcl/random/variate_generator.hpp>
#include <qfcl/random/distribution/gbm_npv_vanilla_call.hpp>

#include <vector>
#include <iostream>

boost::mutex io_mutex;

template <typename Distribution, typename Controller>
struct prf_mc_job
{
private:
    // Private typedefs
    typedef boost::random::philox<4, uint32_t> Prf;
    typedef boost::random::counter_based_urng<Prf> Engine;
    typedef boost::variate_generator<Engine&, Distribution> Sampler;

    // Private variables
    uint32_t        m_sequence_number; // random number sequence ID
    Distribution&   m_distribution;    // The distribution we want to sample from
    Controller&     m_controller;      // The controller that will consume results
public:
    // Constuctor
    prf_mc_job(
        uint32_t sequence_number, Distribution& distribution, Controller& controller)
    :   m_sequence_number(sequence_number), m_distribution(distribution), m_controller(controller)
    {
    }
    
    // Main loop
    void operator()()
    {
        // Initialize parallel random number generator and distribution sampler.
        Prf::domain_type c = {{}};
        Prf::key_type    k = {{m_sequence_number,0}};
        Engine engine(Prf(k), c);
        Sampler sampler(engine, m_distribution);
        
        long samples;
        {
            boost::mutex::scoped_lock lock(io_mutex);
            samples = m_controller.get_samples();
        }
        
        while (samples>0) {
            double sum = 0;
            for (uint32_t i=0; i< samples; ++i)
                sum += sampler();
            
            {
                boost::mutex::scoped_lock lock(io_mutex);
                m_controller.report_result(m_sequence_number, samples, sum);
                samples = m_controller.get_samples();
            }
        }
    }
    
};


template <typename Distribution>
struct job_controller
{
private:
    long m_samples;
    long m_samples_per_job;
    int m_threads;
    int m_jobs;
    
    double m_accumulated_sum;
    long m_accumulated_samples;

public:
    job_controller()
    {
    }
    
    double run(Distribution& distribution, long samples, long jobs, int threads)
    {
        m_samples = samples;
        m_jobs = jobs;
        m_threads = threads;
        m_samples_per_job = ( (m_samples + m_jobs -1) / m_jobs);

        m_accumulated_sum = 0.;
        m_accumulated_samples = 0.;
        
        std::vector< boost::shared_ptr<boost::thread> > threadpool;
        
        for (int i=0; i<m_threads; ++i) {
            threadpool.push_back(
                boost::make_shared<boost::thread>(( prf_mc_job<Distribution,job_controller>(i,distribution,*this) ))
            );
        }

        for (int i=0; i<m_threads; ++i) {
            threadpool[i]->join();
        }
        return m_accumulated_sum/m_accumulated_samples;
    }
    
    long get_samples()
    {
        if (m_samples < m_samples_per_job) {
            long ans = m_samples;
            m_samples = 0;
            return ans;
        }
        m_samples -= m_samples_per_job;
        return m_samples_per_job;
    }
    
    void report_result(int id, long job_samples, double job_sum)
    {
        m_accumulated_samples += job_samples;
        m_accumulated_sum += job_sum;
        /*
        std::cout << id << " reported " << job_samples << " samples, total is now " << m_accumulated_samples 
            << ", estimate=" << m_accumulated_sum/m_accumulated_samples << std::endl;
        */
    }
};



// test123 <samples> <jobs> <threads>
int main(int argc, char *argv[])
{
    if (argc < 4) {
        std::cout << "usage: parallel_monte_carlo <samples> <jobs> <threads>\n";
        std::cout << "       samples: Number of MC samples\n";
        std::cout << "       jobs:    Number of jobs to split the samples into\n";
        std::cout << "       threads: Number of parallel threads to use\n";
        return 1;
    }
    
    long samples = atol(argv[1]);
    int jobs = atoi(argv[2]);
    int threads = atoi(argv[3]);
    
    qfcl::random::gbm_vanilla_call vanila_call(103.50, 0.20, 0.05, 0.05, 100.0, 1.0);
    job_controller< qfcl::random::gbm_vanilla_call > jc;
    
    std::cout << "samples, jobs, threads, duration, result" << std::endl;

        boost::posix_time::ptime time_start(boost::posix_time::microsec_clock::local_time() );
    
        double result = jc.run(vanila_call, samples, jobs, threads);
    
        boost::posix_time::ptime time_end(boost::posix_time::microsec_clock::local_time() );
        boost::posix_time::time_duration duration( time_end - time_start );
        double dt = 0.001* duration.total_milliseconds();
        std::cout << samples << ", " << jobs << ", " << threads << ", " << dt << ", " << result << std::endl;

    return 0;
}
