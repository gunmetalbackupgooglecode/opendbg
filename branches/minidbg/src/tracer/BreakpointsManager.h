#pragma once
#include "Breakpoint.h"
#include <boost/bind.hpp>
#include <list>

namespace debugger{

class BreakpointsManager
{
public:
	BreakpointsManager(void);
	~BreakpointsManager(void);
	void addBreakpoint( debugger::BREAKPOINT_PTR breakpoint );
	void deleteBreakpoint( u3264 memAddress );
	debugger::BREAKPOINT_PTR findBreakpoint( u3264 memAddress) ;

protected:
	std::list<debugger::BREAKPOINT_PTR> breakpoints;
};
}