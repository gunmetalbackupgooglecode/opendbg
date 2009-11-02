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

#include "analyzer.h"

namespace trc
{

	//int analyser::is_instruction_untraceable(INSTRUCTION& instr)
	//{
	//	int result = 0;
	//	if (instr.groups & GRP_STACK)
	//	{
	//		if (instr.id == ID_POPF || instr.id == ID_PUSHF)
	//			result = 1; // Reason: need hide TF flag
	//		else if ((instr.id == ID_POP) && (instr.groups & GRP_SEGREG) && (instr.ops[0].value.reg.code = SREG_CODE_SS))
	//			result = 1; // Reason: POP SS is untraceable command
	//		else if ((instr.groups & GRP_BREAK) && (instr.id != ID_BOUND))
	//			result = 1; // Reason: int's, into, icebp are untraceable commands
	//	}

	//	return result;
	//}

} // namespace trc
