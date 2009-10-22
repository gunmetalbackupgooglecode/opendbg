/*
*
* Copyright (c) october 2009
* Vladimir <progopis@jabber.ru> PGP key ID - 0x59CF2D8A75CB8417
*

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MAIN_H__
#define MAIN_H__
#include <Windows.h>
#include "defines.h"

/* General VM configuration */
#define GEN_REGS_NUM 8
#define GEN_REGS_SIZE_4
#define CTRL_REGS_NUM 0
#define CTRL_REGS_SIZE 0
#define DBG_REGS_NUM 6
#define DBG_REGS_SIZE 4
#define FPU_REGS_NUM 8
#define FPU_REGS_SIZE 10
#define FPU_SPEC_NUM 3
#define FPU_SPEC_SIZE 2
#define STACK_VAR_SIZE_4
#define STACK_REG_INDEX 4
/*//////////////////////////*/

/* Useful types of data */
#ifdef GEN_REGS_SIZE_4
	typedef unsigned int reg_size;
#else
	#ifdef GEN_REGS_SIZE_8
		typedef unsigned long reg_size;
	#else
		typedef unsigned short reg_size;
	#endif
#endif

#ifdef STACK_VAR_SIZE_4
	typedef struct { u32 val;
					u32 type;
					u8 reg_type;
					u32 reg_val;
					} stackvar;
#else
	#ifdef STACK_VAR_SIZE_8
			typedef unsigned long stackvar;
	#else
			typedef unsigned short stackvar;
	#endif
#endif

enum types {TYPE_NUM, TYPE_UNKNOWN_VALUE, TYPE_SEG_REG, TYPE_GEN_REG, TYPE_FPU_REG, TYPE_DBG_REG, TYPE_EXPRESSION};
/*//////////////////////*/

namespace vm
{
class vm_main
{
public:
	typedef struct _graph { int start_offset;
							int brunc_type;
							typedef struct _nodes {struct _nodes * next;
							_graph * node;} nodes;
							} graph;
	typedef struct {reg_size general[GEN_REGS_NUM];
					//ui8 control[CTRL_REGS_NUM][CTRL_REGS_SIZE];
					u8 debug[DBG_REGS_NUM][DBG_REGS_SIZE];
					u8 fpu[FPU_REGS_NUM][FPU_REGS_SIZE];
					u8 fpuSpecials[FPU_SPEC_NUM][FPU_SPEC_SIZE];
					long fpuFlags;
					long Flags;
					long IP;
					} regs;
	typedef struct _stack {	_stack * next;
							stackvar var;
							} * stack;
	typedef struct _seh_frame { struct _seh_frame * next;
								int hanler_offset;
								int safe_offset;
								} seh_frame;

	int init(unsigned char block[]);
	int deobfuscate_nodes();
private:
	graph * main_tree;
	regs registers;
	seh_frame * seh;
	char RegNames[GEN_REGS_NUM][4];
	stack * stk;

	int push (vm_main::stack ** stack, reg_size * ESP, stackvar var);
	int pop (vm_main::stack ** stack, reg_size * ESP, stackvar * var);
	int execute(struct INSTRUCTION instr);
	int seh_do();
};

} // namespace vm

#endif // MAIN_H__