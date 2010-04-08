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

#ifndef BREAKPOINT_H__
#define BREAKPOINT_H__

#include "tracer_error.h"
#include "dbgapi.h"
#include "defines.h"

namespace trc
{

class breakpoint
{
public:
	breakpoint(u32 proc_id, u32 thread_id, u3264 address);
	~breakpoint();

	void turn_off()
	{
		u8 buf;
		u32 readed;
		if (!dbg_read_memory((HANDLE)m_proc_id, (PVOID)m_address, (PVOID)&buf, sizeof(buf), &readed))
			throw tracer_error("can't read memory on destroying breakpoint"); // memory is not readable
		if (INT3_OPCODE == buf)
			if (!dbg_write_memory((HANDLE)m_proc_id, (PVOID)m_address, &m_orig_value, sizeof(m_orig_value), &readed))
				throw tracer_error("can't write memory on destroying breakpoint"); // memory is not writable
	}

	void turn_on()
	{
		u8 buf;
		u32 readed;
		if (!dbg_read_memory((HANDLE)m_proc_id, (PVOID)m_address, (PVOID)&buf, sizeof(buf), &readed))
			throw tracer_error("can't read memory on creating breakpoint"); // memory is not readable

		m_orig_value = buf;
		buf = INT3_OPCODE;
		if (!dbg_write_memory((HANDLE)m_proc_id, (PVOID)m_address, &buf, sizeof(buf), &readed))
			throw tracer_error("can't write memory on creating breakpoint"); // memory is not writable
	}

	int is_enabled()
	{
	}

	u3264 get_address()
	{
		return m_address;
	}

	u32 get_pid()
	{
		return m_proc_id;
	}

	u32 get_tid()
	{
		return m_thread_id;
	}

private:
	u3264 m_address;
	u32   m_proc_id;
	u32   m_thread_id;
	u8   m_orig_value;
};

}

#endif // BREAKPOINT_H__
