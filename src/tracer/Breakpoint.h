/*
*
* Copyright (c) 2009 - 2012
* Vladimir <progopis@gmail.com> PGP key ID - 0x59CF2D8A75CB8417
* Rascal
*

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#ifndef BREAKPOINT_H__
#define BREAKPOINT_H__

//#include "tracer_error.h"
#include "dbgapi.h"
#include "defines.h"
#include "Thread.h"
#include "Process.h"
#include "exceptions.h"

namespace debugger{
	enum break_type{brk_soft, brk_temp, brk_hard};

class Breakpoint
{
public:
	enum ActionType{
		onRead,
		onWrite,
		onAccess,
		onExecute
	};

	Breakpoint(sys::PROCESS_PTR _process, sys::THREAD_PTR _targetThread,
		u3264 _memoryAddress, u3264 _range, ActionType _actionType,
		bool _context_specific) : process(_process), 
		targetThread(_targetThread), memoryAddress(_memoryAddress),
		range(_range), actionType(_actionType), context_specific(_context_specific){
		enabled = false;
		// break on all threads by default
		context_specific = false;
		safe = false;
	}
	virtual ~Breakpoint(void);

	bool is_enabled() const; // is breakpoint turned on
	bool is_context_specific() const; // stop should occur on the
	// specific thread or on any
	bool is_safe() const; // is able to violate the neutrality of debugging
	// (if there is a critical change in the environment, which can lead to
	// problems with checksum or anti-debugging)

	virtual void enable() = 0;
	virtual void disable() = 0;
	virtual break_type get_type() = 0;
	virtual u32 set_break_condition() = 0;

	u3264 getAddress() {
		return memoryAddress;
	}

protected:
	sys::PROCESS_PTR process;
	sys::THREAD_PTR targetThread;
	u3264 memoryAddress;
	u3264 range;
	ActionType actionType;

	bool enabled;
	bool context_specific;
	bool safe;
};

typedef boost::shared_ptr<Breakpoint> BREAKPOINT_PTR;

class HardwareBreakpoint : public Breakpoint{
public:

	// hard break on all threads
	HardwareBreakpoint(sys::PROCESS_PTR _process, sys::THREAD_PTR _targetThread,
		u3264 _memoryAddress, u3264 _range, ActionType _actionType, bool _context_specific):
	  Breakpoint(_process, _targetThread, _memoryAddress, _range, _actionType, _context_specific){
		enable();
	}

	void enable();

	void disable();
	break_type get_type();
	u32 set_break_condition();

protected:

	void enableContextSpecific();
	void enableNonContextSpecific();

	void disableContextSpecific();
	void disableNonContextSpecific();

};


class SoftwareBreakpoint : public Breakpoint{
public:

	// int3 break on all threads
	SoftwareBreakpoint(sys::PROCESS_PTR _process, sys::THREAD_PTR _targetThread,
		u3264 _memoryAddress, u3264 _range, ActionType _actionType, bool _context_specific):
	  Breakpoint(_process, _targetThread, _memoryAddress, _range, _actionType, _context_specific){
		enable();
	}

	~SoftwareBreakpoint() {
		disable();
	}

	void enable();

	void disable();
	break_type get_type();
	u32 set_break_condition();

private:
	u8   m_orig_value;

};
}

#endif // BREAKPOINT_H__
