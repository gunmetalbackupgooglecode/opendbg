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

#include <windows.h>
#include <cstdio>
#include "defines.h"

namespace trc
{

class breakpoint
{
public:
	breakpoint(u32 proc_id, u32 thread_id, u3264 address);
	~breakpoint();

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
};

}

#endif // BREAKPOINT_H__
