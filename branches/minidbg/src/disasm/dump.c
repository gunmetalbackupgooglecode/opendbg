#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "disasm.h"

struct STREAM
{
	char *buff;
	unsigned int bufflen;
	unsigned int reallen;
	unsigned int options;
};

char *regs8[] = 
{
	"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh",
	"r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b",
	"spl", "bpl", "sil", "dil", "badreg8"
};

char *regs16[] = 
{
	"ax", "cx", "dx", "bx", "sp", "bp", "si", "di",
	"r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w",
	"badreg16_1", "badreg16_2", "badreg16_3", "badreg16_4", "badreg16_5",
};

char *regs32[] = 
{
	"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi",
	"r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d",
	"badreg32_1", "badreg32_2", "badreg32_3", "badreg32_4", "eip",
};

char *regs64[] = 
{
	"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
	"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
	"badreg64_1", "badreg64_2", "badreg64_3", "badreg64_4", "rip",
};

char *sregs[] = 
{
	"cs", "ds", "es", "ss", "fs", "gs", "badsreg7", "badsreg8"
};

char *fregs[] = 
{
	"st0", "st1", "st2", "st3", "st4", "st5", "st6", "st7", "badfreg8"
};

char *cregs[] = 
{
	"cr0", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7", "cr8", "badcreg9"
};

char *dregs[] = 
{
	"dr0", "dr1", "dr2", "dr3", "dr4", "dr5", "dr6", "dr7", "baddreg"
};

char *tregs[] = 
{
	"tr0", "tr1", "tr2", "tr3", "tr4", "tr5", "tr6", "tr7", "badtreg"
};

char *mregs[] = 
{
	"mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7", "badmreg8"
};

char *xregs[] = 
{
	"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",
	"xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15",
	"badxreg16"
};

static void di_copy_byte(struct STREAM *stream, char ch)
{
	if (stream ->bufflen)
	{
		*stream ->buff = ch;
		stream ->bufflen--;
		stream ->buff++;
	}
	stream ->reallen++;
}

static void di_strncpy(struct STREAM *stream, const char *src)
{
	while (*src)
	{
		di_copy_byte(stream, *src);
		src++;
	}
}

static void itoa(struct STREAM *stream, uint64_t num, uint16_t size, int is_signed, int radix)
{
	char dig;
	uint64_t div;
	uint64_t tmp;

	div = 1;
	if (is_signed)
	{
		uint8_t *ptr = (uint8_t *)&num;
		ptr += size - 1;
		if (*ptr & 0x80)
		{
			di_copy_byte(stream, '-');
			num = ~num; //Just to shut up warning.
			num++;
		}
	}

	tmp = 0xFFFFFFFFFFFFFFFFL;
	for(size = 8 - size + 1; size; size--)
	{
		num &= tmp;
		tmp >>= 0x8;
	}

	if (radix == 0x10)
		di_strncpy(stream, "0x");

	while(num / div >= radix)
		div *= radix;
	while(div)
	{
		dig = (uint8_t)((num / div) % radix);
		if (dig >= 10)
			dig += 'A' - 10;
		else
			dig += '0';
		di_copy_byte(stream, dig);
		div /= radix;
	}
}

static void dump_imm(struct STREAM *stream, struct OPERAND *op)
{
	switch(stream ->options & OPTION_IMM_MASK)
	{
	case OPTION_IMM_HEX:
		itoa(stream, op ->value.imm.imm64, op ->size, 0x1, 0x10);
		break;
	case OPTION_IMM_UHEX:
		itoa(stream, op ->value.imm.imm64, op ->size, 0x0, 0x10);
		break;
	case OPTION_IMM_DEC:
		itoa(stream, op ->value.imm.imm64, op ->size, 0x0, 0xA);
		break;
	case OPTION_IMM_UDEC:
		itoa(stream, op ->value.imm.imm64, op ->size, 0x1, 0xA);
		break;
	default:
		itoa(stream, op ->value.imm.imm64, op ->size, 0x0, 0x10);
		break;
	}
}

static void dump_reg_sreg(struct STREAM *stream, uint8_t code)
{
	di_strncpy(stream, sregs[code]);
}

static void dump_reg_gen(struct STREAM *stream, uint8_t code, uint16_t size)
{
	switch(size)
	{
	case OPERAND_SIZE_8:
		di_strncpy(stream, regs8[code]);
		break;
	case OPERAND_SIZE_16:
		di_strncpy(stream, regs16[code]);
		break;
	case OPERAND_SIZE_32:
		di_strncpy(stream, regs32[code]);
		break;
	case OPERAND_SIZE_64:
		di_strncpy(stream, regs64[code]);
		break;
	}
}

static void dump_reg_freg(struct STREAM *stream, uint8_t code)
{
	di_strncpy(stream, fregs[code]);
}

static void dump_reg_creg(struct STREAM *stream, uint8_t code)
{
	di_strncpy(stream, cregs[code]);
}

static void dump_reg_dreg(struct STREAM *stream, uint8_t code)
{
	di_strncpy(stream, dregs[code]);
}

static void dump_reg_treg(struct STREAM *stream, uint8_t code)
{
	di_strncpy(stream, tregs[code]);
}

static void dump_reg_mreg(struct STREAM *stream, uint8_t code)
{
	di_strncpy(stream, mregs[code]);
}

static void dump_reg_xreg(struct STREAM *stream, uint8_t code)
{
	di_strncpy(stream, xregs[code]);
}

static void dump_reg(struct STREAM *stream, uint8_t type, uint8_t code, uint16_t size)
{
	switch(type)
	{
	case REG_TYPE_GEN:
		dump_reg_gen(stream, code, size);
		break;
	case REG_TYPE_SEG:
		dump_reg_sreg(stream, code);
		break;
	case REG_TYPE_FPU:
		dump_reg_freg(stream, code);
		break;
	case REG_TYPE_CR:
		dump_reg_creg(stream, code);
		break;
	case REG_TYPE_DBG:
		dump_reg_dreg(stream, code);
		break;
	case REG_TYPE_TR:
		dump_reg_treg(stream, code);
		break;
	case REG_TYPE_MMX:
		dump_reg_mreg(stream, code);
		break;
	case REG_TYPE_XMM:
		dump_reg_xreg(stream, code);
		break;
	default:
		di_strncpy(stream, "internal error");
		break;
	}
}

static void dump_disp(struct STREAM *stream, uint8_t mod, struct INSTRUCTION *instr)
{
	unsigned int options;

	options = stream ->options;
	if (mod == ADDR_MOD_BASE)
	{
		switch(options & OPTION_DISP_MASK)
		{
		case OPTION_DISP_HEX:
			options &= ~OPTION_DISP_HEX;
			options |= OPTION_DISP_UHEX;
			break;
		case OPTION_DISP_DEC:
			options &= ~OPTION_DISP_DEC;
			options |= OPTION_DISP_UDEC;
			break;
		}
	}

	switch(options & OPTION_DISP_MASK)
	{
	case OPTION_DISP_HEX:
		itoa(stream, instr ->disp.value.d64, instr ->addrsize, 0x1, 0x10);
		break;
	case OPTION_DISP_UHEX:
		itoa(stream, instr ->disp.value.d64, instr ->addrsize, 0x0, 0x10);
		break;
	case OPTION_DISP_DEC:
		itoa(stream, instr ->disp.value.d64, instr ->addrsize, 0x1, 0xA);
		break;
	case OPTION_DISP_UDEC:
		itoa(stream, instr ->disp.value.d64, instr ->addrsize, 0x0, 0xA);
		break;
	default:
		itoa(stream, instr ->disp.value.d64, instr ->addrsize, 0x0, 0x10);
		break;
	}
}

static void dump_ptr_size(struct STREAM *stream, struct OPERAND *op)
{
	switch(op ->size)
	{
	case OPERAND_SIZE_8:
		di_strncpy(stream, "byte ptr");
		break;
	case OPERAND_SIZE_16:
		di_strncpy(stream, "word ptr");
		break;
	case OPERAND_SIZE_32:
		di_strncpy(stream, "dword ptr");
		break;
	case OPERAND_SIZE_48:
		di_strncpy(stream, "fword ptr");
		break;
	case OPERAND_SIZE_64:
		di_strncpy(stream, "qword ptr");
		break;
	case OPERAND_SIZE_80:
		di_strncpy(stream, "tbyte ptr");
		break;
	case OPERAND_SIZE_128:
		di_strncpy(stream, "dqword ptr");
		break;
	case OPERAND_SIZE_14:
		di_strncpy(stream, "14bytes ptr");
		break;
	case OPERAND_SIZE_28:
		di_strncpy(stream, "28bytes ptr");
		break;
	case OPERAND_SIZE_94:
		di_strncpy(stream, "94bytes ptr");
		break;
	case OPERAND_SIZE_108:
		di_strncpy(stream, "108bytes ptr");
		break;
	case OPERAND_SIZE_512:
		di_strncpy(stream, "512bytes ptr");
		break;
	default:
		di_strncpy(stream, "strange ptr");
		break;
	}
}

static void dump_addr(struct STREAM *stream, struct INSTRUCTION *instr, struct OPERAND *op)
{
	dump_ptr_size(stream, op);

	di_copy_byte(stream, ' ');

	di_strncpy(stream, sregs[op ->value.addr.seg]);
	di_copy_byte(stream, ':');
	di_copy_byte(stream, '[');
	if (op ->value.addr.mod & ADDR_MOD_BASE)
	{
		dump_reg_gen(stream, op ->value.addr.base, instr ->addrsize);
	}
	if (op ->value.addr.mod & ADDR_MOD_IDX)
	{
		if (op ->value.addr.mod & ADDR_MOD_BASE)
		{
			di_copy_byte(stream, '+');
		}
		dump_reg_gen(stream, op ->value.addr.index, instr ->addrsize);
		if (op ->value.addr.scale != 0x1)
		{
			di_copy_byte(stream, '*');
			itoa(stream, op ->value.addr.scale, 0x1, 0x0, 0xA);
		}
	}
	if (op ->value.addr.mod & ADDR_MOD_DISP)
	{
		if (op ->value.addr.mod & ~ADDR_MOD_DISP)
		{
			if ((stream ->options & OPTION_DISP_UHEX) || (stream ->options & OPTION_DISP_UDEC))
			{
				di_copy_byte(stream, '+');
			}
			else
			{
				uint8_t *ptr = (uint8_t *)(&(instr ->disp.value));
				ptr += instr ->addrsize - 1;

				if (!(*ptr & 0x80))
				{
					di_copy_byte(stream, '+');
				}
			}
		}

		dump_disp(stream, op ->value.addr.mod, instr);
	}

	di_copy_byte(stream, ']');
}

static void dump_dir(struct STREAM *stream, struct OPERAND *op)
{
	switch (op ->size)
	{
	case OPERAND_SIZE_32:
		itoa(stream, op ->value.far_addr.far_addr32.seg, 0x2, 0x0, 0x10);
		di_copy_byte(stream, ':');
		itoa(stream, op ->value.far_addr.far_addr32.offset, 0x2, 0x0, 0x10);
		break;
	case OPERAND_SIZE_48:
		itoa(stream, op ->value.far_addr.far_addr48.seg, 0x2, 0x0, 0x10);
		di_copy_byte(stream, ':');
		itoa(stream, op ->value.far_addr.far_addr48.offset, 0x4, 0x0, 0x10);
		break;
	default:
		di_strncpy(stream, "internal error");
		break;
	}
}

static int internal_dump_operand(struct STREAM *stream, struct INSTRUCTION *instr, int op_index)
{
	struct OPERAND *op;

	if (op_index > 3 || op_index < 0)
		return -1;

	op = instr ->ops + op_index;
	if (op ->flags & OPERAND_PRESENT)
	{
		switch (op ->flags & 0xFE)
		{
		case OPERAND_TYPE_REG:
			dump_reg(stream, op ->value.reg.type, op ->value.reg.code, op ->size);
			break;
		case OPERAND_TYPE_MEM:
			dump_addr(stream, instr, op);
			break;
		case OPERAND_TYPE_IMM:
			dump_imm(stream, op);
			break;
		case OPERAND_TYPE_DIR:
			dump_dir(stream, op);
			break;
		default:
			di_strncpy(stream, "internal error");
			break;
		}
	}

	return stream ->reallen;
}

int dump_operand(struct INSTRUCTION *instr, int op_index, int options, char *buff, int len)
{
	struct STREAM stream;

	stream.buff = buff;
	stream.bufflen = len;
	stream.reallen = 0;
	stream.options = options;

	return internal_dump_operand(&stream, instr, op_index);
}

static int internal_dump_instruction(struct STREAM *stream, struct INSTRUCTION *instr)
{
	di_strncpy(stream, instr ->mnemonic);

	return stream ->reallen;
}

int dump_instruction(struct INSTRUCTION *instr, char *buff, int len)
{
	struct STREAM stream;

	stream.buff = buff;
	stream.bufflen = len;
	stream.reallen = 0;

	internal_dump_instruction(&stream, instr);

	return stream.reallen;
}

static int internal_dump_prefixes(struct STREAM *stream, struct INSTRUCTION *instr)
{
	if (instr ->prefixes & INSTR_PREFIX_LOCK)
		di_strncpy(stream, "lock ");
	if (instr ->prefixes & INSTR_PREFIX_REPZ)
		di_strncpy(stream, "repz ");
	if (instr ->prefixes & INSTR_PREFIX_REPNZ)
		di_strncpy(stream, "repnz ");

	return stream ->reallen;
}

int dump_prefixes(struct INSTRUCTION *instr, int options, char *buff, int bufflen)
{
	struct STREAM stream;

	stream.buff = buff;
	stream.bufflen = bufflen;
	stream.options = options;
	stream.reallen = 0;

	return internal_dump_prefixes(&stream, instr);
}

int dump(struct INSTRUCTION *instr, char *buff, int bufflen, int options)
{
	struct STREAM stream;

	stream.buff = buff;
	stream.bufflen = bufflen;
	stream.options = options;
	stream.reallen = 0;

	internal_dump_prefixes(&stream, instr);
	internal_dump_instruction(&stream, instr);

	if (instr ->ops[0].flags & OPERAND_PRESENT)
	{
		unsigned int i;

		di_copy_byte(&stream, ' ');
		for (i = 0; i < 3; i++)
		{
			if (instr ->ops[i].flags & OPERAND_PRESENT)
			{
				internal_dump_operand(&stream, instr, i);
				di_strncpy(&stream, ", ");
			}
		}
		stream.reallen -= 2;
		stream.bufflen += 2;
		stream.buff += 2;
	}

	return stream.reallen;
}


static void dump_sf_prefixes(struct STREAM *stream, uint8_t *sf_prefixes)
{
	unsigned int i, count;

	if (!sf_prefixes)
	{
		di_strncpy(stream, "unknown");
	}
	else
	{
		count = (unsigned int)strlen((char*)sf_prefixes);
		if (!count)
			di_strncpy(stream, "none");
		else
		{
			for(i = 0; i < count; i++)
			{
				itoa(stream, sf_prefixes[i], 0x1, 0x0, 0x10);
				di_strncpy(stream, ", ");
			}
		}
	}
}

static void dump_prefixes_dbg(struct STREAM *stream, struct INSTRUCTION *instr)
{
	if (!instr ->prefixes)
		di_strncpy(stream, "none");
	else
	{
		if (instr ->prefixes & INSTR_PREFIX_CS)
			di_strncpy(stream, "CS, ");
		if (instr ->prefixes & INSTR_PREFIX_DS)
			di_strncpy(stream, "DS, ");
		if (instr ->prefixes & INSTR_PREFIX_ES)
			di_strncpy(stream, "ES, ");
		if (instr ->prefixes & INSTR_PREFIX_SS)
			di_strncpy(stream, "SS, ");
		if (instr ->prefixes & INSTR_PREFIX_FS)
			di_strncpy(stream, "FS, ");
		if (instr ->prefixes & INSTR_PREFIX_GS)
			di_strncpy(stream, "GS, ");
		if (instr ->prefixes & INSTR_PREFIX_OPSIZE)
			di_strncpy(stream, "OPSIZE, ");
		if (instr ->prefixes & INSTR_PREFIX_ADDRSIZE)
			di_strncpy(stream, "ADDRSIZE, ");
		if (instr ->prefixes & INSTR_PREFIX_REX)
		{
			di_strncpy(stream, "REX (");
			itoa(stream, instr ->rex, 0x1, 0x0, 0x10);
			di_strncpy(stream, "), ");
		}
		if (instr ->prefixes & INSTR_PREFIX_LOCK)
			di_strncpy(stream, "LOCK, ");
		stream ->bufflen += 2;
		stream ->buff -= 2;
		stream ->reallen -= 2;
	}
}

static void dump_flags(struct STREAM *stream, struct INSTRUCTION *instr)
{
	if (!instr ->flags)
		di_strncpy(stream, "none");
	else
	{
		if (instr ->flags & INSTR_FLAG_MODRM)
		{
			di_strncpy(stream, "MODRM (");
			itoa(stream, instr ->modrm, 0x1, 0x0, 0x10);
			di_strncpy(stream, "), ");
		}
		if (instr ->flags & INSTR_FLAG_SIB)
		{
			di_strncpy(stream, "SIB (");
			itoa(stream, instr ->sib, 0x1, 0x0, 0x10);
			di_strncpy(stream, "), ");
		}
		if (instr ->flags & INSTR_FLAG_D64)
			di_strncpy(stream, "D64, ");
		if (instr ->flags & INSTR_FLAG_F64)
			di_strncpy(stream, "F64, ");
		if (instr ->flags & INSTR_FLAG_SUPERFLUOUS_PREFIX)
			di_strncpy(stream, "SF_PREFIXES, ");
		stream ->bufflen += 2;
		stream ->buff -= 2;
		stream ->reallen -= 2;
	}
}

static void dump_eflags(struct STREAM *stream, uint8_t flags)
{
	if (!flags)
		di_strncpy(stream, "none");
	else
	{
		if (flags & EFLAG_O)
			di_strncpy(stream, "OF, ");
		if (flags & EFLAG_D)
			di_strncpy(stream, "DF, ");
		if (flags & EFLAG_I)
			di_strncpy(stream, "IF, ");
		if (flags & EFLAG_S)
			di_strncpy(stream, "SF, ");
		if (flags & EFLAG_Z)
			di_strncpy(stream, "ZF, ");
		if (flags & EFLAG_A)
			di_strncpy(stream, "AF, ");
		if (flags & EFLAG_P)
			di_strncpy(stream, "PF, ");
		if (flags & EFLAG_C)
			di_strncpy(stream, "CF, ");
		stream ->bufflen += 2;
		stream ->buff -= 2;
		stream ->reallen -= 2;
	}
}

void dump_groups(struct STREAM *stream, struct INSTRUCTION *instr)
{
	if (!instr ->groups)
		di_strncpy(stream, "none");
	else
	{
		if (instr ->groups & GRP_GEN)
			di_strncpy(stream, "GEN, ");
		if (instr ->groups & GRP_SSE1)
			di_strncpy(stream, "SSE1, ");
		if (instr ->groups & GRP_ARITH)
			di_strncpy(stream, "ARITH, ");
		if (instr ->groups & GRP_BINARY)
			di_strncpy(stream, "BINARY, ");
		if (instr ->groups & GRP_STACK)
			di_strncpy(stream, "STACK, ");
		if (instr ->groups & GRP_SEGREG)
			di_strncpy(stream, "SEGREG, ");
		if (instr ->groups & GRP_LOGICAL)
			di_strncpy(stream, "LOGICAL, ");
		if (instr ->groups & GRP_PREFIX)
			di_strncpy(stream, "PREFIX, ");
		if (instr ->groups & GRP_BRANCH)
			di_strncpy(stream, "BRANCH, ");
		if (instr ->groups & GRP_COND)
			di_strncpy(stream, "COND, ");
		if (instr ->groups & GRP_DECIMAL)
			di_strncpy(stream, "DECIMAL, ");
		if (instr ->groups & GRP_BREAK)
			di_strncpy(stream, "BREAK, ");
		if (instr ->groups & GRP_SYSTEM)
			di_strncpy(stream, "SYSTEM, ");
		if (instr ->groups & GRP_INOUT)
			di_strncpy(stream, "INOUT, ");
		if (instr ->groups & GRP_STRING)
			di_strncpy(stream, "STRING, ");
		if (instr ->groups & GRP_DATAMOV)
			di_strncpy(stream, "DATAMOV, ");
		if (instr ->groups & GRP_CONTROL)
			di_strncpy(stream, "CONTROL, ");
		if (instr ->groups & GRP_CONVER)
			di_strncpy(stream, "CONVER, ");
		if (instr ->groups & GRP_X87FPU)
			di_strncpy(stream, "X87FPU, ");
		if (instr ->groups & GRP_FLGCTRL)
			di_strncpy(stream, "FLGCTRL, ");
		if (instr ->groups & GRP_SHFTROT)
			di_strncpy(stream, "SHFTROT, ");
		if (instr ->groups & GRP_SIMDFP)
			di_strncpy(stream, "SIMDFP, ");
		if (instr ->groups & GRP_SHUNPCK)
			di_strncpy(stream, "SHUNPCK, ");
		if (instr ->groups & GRP_FETCH)
			di_strncpy(stream, "FETCH, ");
		if (instr ->groups & GRP_CACHECT)
			di_strncpy(stream, "CACHECT, ");
		if (instr ->groups & GRP_COMPAR)
			di_strncpy(stream, "COMPAR, ");
		if (instr ->groups & GRP_TRANS)
			di_strncpy(stream, "TRANS, ");
		if (instr ->groups & GRP_SIMDINT)
			di_strncpy(stream, "SIMDINT, ");
		if (instr ->groups & GRP_PCKSCLR)
			di_strncpy(stream, "PCKSCLR, ");
		if (instr ->groups & GRP_PCKSP)
			di_strncpy(stream, "PCKSP, ");
		if (instr ->groups & GRP_UNPACK)
			di_strncpy(stream, "UNPACK, ");
		if (instr ->groups & GRP_SHIFT)
			di_strncpy(stream, "SHIFT, ");
		if (instr ->groups & GRP_BIT)
			di_strncpy(stream, "BIT, ");
		if (instr ->groups & GRP_SM)
			di_strncpy(stream, "SM, ");
		if (instr ->groups & GRP_MXCSRSM)
			di_strncpy(stream, "MXCSRSM, ");
		if (instr ->groups & GRP_ORDER)
			di_strncpy(stream, "ORDER, ");
		if (instr ->groups & GRP_SSE2)
			di_strncpy(stream, "SSE2, ");
		if (instr ->groups & GRP_SSSE3)
			di_strncpy(stream, "SSSE3, ");
		if (instr ->groups & GRP_MMX)
			di_strncpy(stream, "MMX, ");
		if (instr ->groups & GRP_SSE3)
			di_strncpy(stream, "SSE3, ");
		if (instr ->groups & GRP_SMX)
			di_strncpy(stream, "GRP_SMX, ");
		if (instr ->groups & GRP_VMX)
			di_strncpy(stream, "GRP_VMX, ");
		if (instr ->groups & GRP_SSE41)
			di_strncpy(stream, "GRP_SSE41, ");
		if (instr ->groups & GRP_SSE42)
			di_strncpy(stream, "GRP_SSE42, ");
		if (instr ->groups & GRP_STRTXT)
			di_strncpy(stream, "GRP_STRTXT, ");

		stream ->bufflen += 2;
		stream ->buff -= 2;
		stream ->reallen -= 2;
	}
}

int dump_dbg(struct INSTRUCTION *instr, uint8_t *sf_prefixes, char *buff, unsigned int len)
{
	unsigned int i;
	struct STREAM stream;

	stream.buff = buff;
	stream.bufflen = len;
	stream.reallen = 0;
	stream.options = (OPTION_DISP_UHEX | OPTION_IMM_UHEX);

	di_strncpy(&stream, "Superfluous prefixes:\n\t");
	dump_sf_prefixes(&stream, sf_prefixes);
	di_copy_byte(&stream, '\n');

	di_strncpy(&stream, "Prefixes:\n\t");
	dump_prefixes_dbg(&stream, instr);
	di_copy_byte(&stream, '\n');

	di_strncpy(&stream, "Mnemonic:\n\t");
	di_strncpy(&stream, "************\n\t");
	internal_dump_instruction(&stream, instr);
	di_strncpy(&stream, "\n\t************");
	di_copy_byte(&stream, '\n');

	di_strncpy(&stream, "Instruction bytes offset:\n\t");
	itoa(&stream, instr ->opcode_offset, 0x1, 0x0, 0x10);
	di_copy_byte(&stream, '\n');

	di_strncpy(&stream, "Flags:\n\t");
	dump_flags(&stream, instr);
	di_copy_byte(&stream, '\n');
/*
	di_strncpy(&stream, "SIB:\n\t");
	if (instr ->flags & INSTR_FLAG_SIB)
		itoa(&stream, instr ->sib, 0x1, 0x0, 0x10);
	else
		di_strncpy(&stream, "none");
	di_copy_byte(&stream, '\n');

	di_strncpy(&stream, "REX:\n\t");
	if (instr ->flags & INSTR_PREFIX_REX)
		itoa(&stream, instr ->rex, 0x1, 0x0, 0x10);
	else
		di_strncpy(&stream, "none");
	di_copy_byte(&stream, '\n');
*/
	for (i = 0; i < 3; i++)
	{
		di_strncpy(&stream, "OPERAND ");
		itoa(&stream, i, sizeof(i), 0, 0x10);
		di_strncpy(&stream, ":\n\t");
		if ( !(instr ->ops[i].flags & OPERAND_PRESENT) )
		{
			di_strncpy(&stream, "not present");
		}
		else
		{
			internal_dump_operand(&stream, instr, i);
			if (instr ->ops[i].flags & OPERAND_TYPE_IMM)
			{
				di_strncpy(&stream, "; ");
				di_strncpy(&stream, "Immediate offset: ");
				itoa(&stream, instr ->ops[i].value.imm.offset, 0x1, 0x0, 0x10);
			}
			if ( (instr ->ops[i].flags & OPERAND_TYPE_MEM) && (instr ->ops[i].value.addr.mod & ADDR_MOD_DISP) )
			{
				di_strncpy(&stream, ";	");
				di_strncpy(&stream, "Displacement offset: ");
				itoa(&stream, instr ->disp.offset, 0x1, 0x0, 0x10);
			}
		}
		di_copy_byte(&stream, '\n');
	}
	
	di_strncpy(&stream, "Tested flags:\n\t");
	dump_eflags(&stream, instr ->tested_flags);
	di_copy_byte(&stream, '\n');

	di_strncpy(&stream, "Modified flags:\n\t");
	dump_eflags(&stream, instr ->modified_flags);
	di_copy_byte(&stream, '\n');

	di_strncpy(&stream, "Set flags:\n\t");
	dump_eflags(&stream, instr ->set_flags);
	di_copy_byte(&stream, '\n');

	di_strncpy(&stream, "Cleared flags:\n\t");
	dump_eflags(&stream, instr ->cleared_flags);
	di_copy_byte(&stream, '\n');

	di_strncpy(&stream, "Undefined flags:\n\t");
	dump_eflags(&stream, instr ->undefined_flags);
	di_copy_byte(&stream, '\n');

	di_strncpy(&stream, "Groups:\n\t");
	dump_groups(&stream, instr);

	return stream.reallen;
}