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

#include "precompiled.h"
#include "breakpoint.h"

namespace trc
{

breakpoint::breakpoint(u32 proc_id, u32 thread_id, u3264 address)
	: m_proc_id(proc_id),
	  m_thread_id(thread_id),
	  m_address(address)
{
	std::cout << "breakpoint ctor\n";
	turn_on();
}

breakpoint::~breakpoint()
{
	std::cout << "breakpoint dtor\n";
}

}
