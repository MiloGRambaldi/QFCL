// MCTypeDMediator.hpp
//
// Control class for the one-factor SDE and its
// approximation in FDM.
//
// Clients (e.g. a main program) use this class as a 
// Facade because it has a well-defined interface.
// The mediatoir uses a factory to initialise all relevant
// variables.
//
// 2007-3-10 DD Kick-off and initial version
// 2007-7-24 DD Debugging
// 2011-12-5 DD payoff --> signals
// 2011-12-9 DD use uBLAS vector
// 2011-12-11 DD generic RMG class from Boost
//
// This class plays the role of the Director in the Builder
// pattern (if we decide to use it).
//
// (C) Datasim Education BV 2007-2011
//

#ifndef MCTypeDMediator_HPP
#define MCTypeDMediator_HPP

#include <utility>

#include <boost/random.hpp>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>			// Sending to I/O stream.

#include "MCPostProcess.hpp"
#include "Sde.hpp"
#include "FDMVisitor.hpp"

#include <boost/function.hpp>
#include <boost/signals.hpp>
#include <boost/tuple/tuple.hpp>

// Namespaces typedefs etc.

namespace ublas=boost::numeric::ublas;

template<typename Counter>
struct Progress
{
	typedef boost::signal<void (Counter index)> signal;	// The progress bar (can be ON/OFF)
	typedef boost::function<void (Counter val)> function;
};

// Payoff must be a functor with signature: Real (Real)
template <typename Real, typename Counter, typename Generator, typename Payoff, typename MCReporter>
class MCTypeDMediator
{
	public:
		//typedef boost::function<Real (Real)> Payoff;
		//typedef boost::tuple< FdmVisitor<Real, Real, Real, Generator>,
		//					  MCReporter,
		//					  Payoff, 
		//					  Progress<Counter>::function > System;

		MCTypeDMediator(FdmVisitor<Real, Real, Real, Generator> & myFdm, MCReporter & _mcr,
						Counter NSimulations, const Payoff & optionPayoff, 
						const typename Progress<Counter>::function & progress, bool show_progress)
						: mcr(_mcr), payoff(optionPayoff)

		{
			NSim = NSimulations;
			if (show_progress == true)
			{
				prog.connect(progress);
			}
			
			fdm = &myFdm;			
		}

		void price()
		{
			// A. Loop over each iteration
			// B. Find value at t = T
			// C. Return the vectors and calc. payoff vectors + average
			// D. Discount the value

			
			// ??? JHH
			// Create the random numbers
			//counter N = fdm->N;
		
			ublas::vector<Real> TerminalValue(NSim, 0.0); // Array of values at t = T
	
			// A.
			for (Counter i = 0; i < NSim; ++i)
			{ // Calculate a path at each iteration
		
				prog(i);
			
				// Compute the current path and get value at t = T
				// For more complicated payoffs we have to send the complete path.
				
				//TerminalValue[i] = payoff(fdm->path()[fdm->path().size()-1]);
				TerminalValue[i] = payoff(fdm -> path());
			}
	
			// Send statistics Display information
	
			// V2: signals2
			boost::signal<void (Status)> slotControl;
			boost::signal<void (const boost::numeric::ublas::vector<Real>& arr)> slotData;

			// Connect signals to slots. N.B. use Boost references, otherwise a copy is
			// made and you will get incorrect results.
			slotControl.connect(boost::ref(mcr)); // Create a reference to mcr
			slotData.connect(boost::ref(mcr));

			//  C. Take the average; price will be in the slot
			slotControl(START);						// Signal to start process, timer
				slotData(TerminalValue);			// Marshall computed data to postprocessor	
			slotControl(STOP);						// Signal to stop receiving data
				
		}
	private:
		Counter NSim;				// Number of simulations, needed for discounting

		Payoff payoff;

		typename Progress<Counter>::signal prog;
		FdmVisitor<Real,Real,Real,Generator> * fdm;
		MCReporter & mcr;
};

#endif
