#include "BreakpointsManager.h"

namespace debugger{

BreakpointsManager::BreakpointsManager(void)
{
}

BreakpointsManager::~BreakpointsManager(void)
{
}

void BreakpointsManager::addBreakpoint( BREAKPOINT_PTR breakpoint )
{
	breakpoints.push_back(breakpoint);
}

debugger::BREAKPOINT_PTR BreakpointsManager::findBreakpoint( u3264 memAddress )
{
	std::list<debugger::BREAKPOINT_PTR>::iterator breakpoint_it;
	breakpoint_it = std::find_if(
		breakpoints.begin(),
		breakpoints.end(),
		boost::bind(&Breakpoint::getAddress, _1) == memAddress
	);
	if (breakpoint_it != breakpoints.end()) {
		return *breakpoint_it;
	}
	return debugger::BREAKPOINT_PTR();
}

void BreakpointsManager::deleteBreakpoint( u3264 memAddress )
{
	std::remove_if(
		breakpoints.begin(),
		breakpoints.end(),
		boost::bind(&Breakpoint::getAddress, _1) == memAddress
	);
}

}