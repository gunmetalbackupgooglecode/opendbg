/*
 *
 * Copyright (c) 2009
 * Vladimir <progopis@jabber.ru> PGP key ID - 0x59CF2D8A75CB8417
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

#include "stdafx.h"
#include "Breakpoint.h"
#include <boost/bind.hpp>
#include <algorithm>

namespace debugger{


Breakpoint::~Breakpoint(void)
{
}

bool Breakpoint::is_enabled() const
{
	return enabled;
}

bool Breakpoint::is_context_specific() const
{
	return context_specific;
}

bool Breakpoint::is_safe() const
{
	return safe;
}

void HardwareBreakpoint::enable()
{
	if(context_specific)
		enableContextSpecific();
	else
		enableNonContextSpecific();
}

void HardwareBreakpoint::disable()
{
	
}

break_type HardwareBreakpoint::get_type()
{
	return brk_hard;
}

u32 HardwareBreakpoint::set_break_condition()
{
	return 0;
}

void HardwareBreakpoint::enableContextSpecific()
{
	//if(targetThread->haveEmptyDebugRegister())
	BOOST_THROW_EXCEPTION(NoEmptyDebugResgisterException());
}

void HardwareBreakpoint::enableNonContextSpecific()
{
	int threadNum = process->getThreadManager().threadNum();
	const std::list<sys::THREAD_PTR>& threads = process->getThreadManager().getThreads();
	std::list<sys::THREAD_PTR>::const_iterator theadWithoutFreeDbgRegs;
	theadWithoutFreeDbgRegs = std::find_if(threads.begin(), threads.end(), 
		boost::bind(
			std::logical_not<bool>(), 
			boost::bind(&sys::Thread::haveEmptyDebugRegister, _1)));
	if(theadWithoutFreeDbgRegs != threads.end())
		BOOST_THROW_EXCEPTION(NoEmptyDebugResgisterException());
}

void SoftwareBreakpoint::enable()
{
	u3264 m_proc_id = process->getPid();
	
	u8 buf = 0;
	u32 readed = 0;
	if (!dbg_read_memory((HANDLE)m_proc_id, (PVOID)memoryAddress, (PVOID)&buf, sizeof(buf), &readed))
		BOOST_THROW_EXCEPTION(CantReadMemoryException()); // memory is not readable

	m_orig_value = buf;
	buf = INT3_OPCODE;
	if (!dbg_write_memory((HANDLE)m_proc_id, (PVOID)memoryAddress, &buf, sizeof(buf), &readed))
		BOOST_THROW_EXCEPTION(CantWriteMemoryException()); // memory is not writable
}

void SoftwareBreakpoint::disable()
{
	u3264 m_proc_id = process->getPid();
	u8 buf = 0;
	u32 readed = 0;

	if (!dbg_read_memory((HANDLE)m_proc_id, (PVOID)memoryAddress, (PVOID)&buf, sizeof(buf), &readed))
		BOOST_THROW_EXCEPTION(CantReadMemoryException()); // memory is not readable
	if (INT3_OPCODE == buf)
	{
		if (!dbg_write_memory((HANDLE)m_proc_id, (PVOID)memoryAddress, &m_orig_value, sizeof(m_orig_value), &readed))
		BOOST_THROW_EXCEPTION(CantWriteMemoryException()); // memory is not writable
	}
}

break_type SoftwareBreakpoint::get_type()
{
	return brk_soft;
}

u32 SoftwareBreakpoint::set_break_condition()
{
	return 0;
}

}
