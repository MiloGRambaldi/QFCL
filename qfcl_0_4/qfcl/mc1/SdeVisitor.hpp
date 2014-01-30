// SdeVisitor.hpp
//
// Base class for all One Factor Sde visitors
//
//	DD 2007-3-3 Kick-off code
//	DD 2007-7-23 review; Support for type A only here
//  DD 2007-7-27 Type D now supported
//  2011-129 DD templated version
//
// (C) Datasim Education BV 1998-2011

#ifndef SdeVisitor_hpp
#define SdeVisitor_hpp

#include "Sde.hpp"


template <typename X, typename Time, typename RT>
			class SdeVisitor
{
private:

protected:
		Sde<X,Time,RT>* sde;
public:
	// Constructors and Destructor
	SdeVisitor() { sde = nullptr; }						
	SdeVisitor(Sde<X,Time,RT>& source) { sde = &source;}	
	virtual ~SdeVisitor() {}				
	
	// The visit functions
	virtual void Visit(Sde<X,Time,RT>& source) = 0;	

	// Operators
	SdeVisitor<X,Time,RT>& operator = (const SdeVisitor<X,Time,RT>& source) {}	// Assignment
};

#endif	// SdeVisitor_hpp
