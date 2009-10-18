#ifndef DISASM_H_
#define DISASM_H_
#include "pstdint.h"

/*************************
* Optional Unicode support
**************************
*/
#ifdef UNICODE
typedef wchar_t unichar_t;
#define _UT(s) L##s
#else
typedef char unichar_t;
#define _UT(s) s
#endif

/************************
* Defines and structs for
* operand.
*************************
*/
#define	OPERAND_PRESENT  0x1

#define	OPERAND_TYPE_REG 0x2
#define	OPERAND_TYPE_MEM 0x4
#define	OPERAND_TYPE_IMM 0x8
#define OPERAND_TYPE_DIR 0x10

#define	OPERAND_SIZE_8    0x1
#define	OPERAND_SIZE_16   0x2
#define	OPERAND_SIZE_32   0x4
#define	OPERAND_SIZE_48   0x6
#define	OPERAND_SIZE_64   0x8
#define	OPERAND_SIZE_80   0xA
#define	OPERAND_SIZE_128  0x10
#define OPERAND_SIZE_14   0xE
#define OPERAND_SIZE_28   0x1C
#define OPERAND_SIZE_94   0x5E
#define OPERAND_SIZE_108  0x6C
#define OPERAND_SIZE_512  0x200

#define ADDR_MOD_BASE 0x1
#define ADDR_MOD_IDX  0x2
#define ADDR_MOD_DISP 0x4

#define REG_TYPE_GEN 0x0
#define REG_TYPE_SEG 0x1
#define REG_TYPE_CR  0x2
#define REG_TYPE_DBG 0x3
#define REG_TYPE_TR  0x4
#define REG_TYPE_FPU 0x5
#define REG_TYPE_MMX 0x7
#define REG_TYPE_XMM 0x8

#define REG_CODE_AX  0x0
#define REG_CODE_CX  0x1
#define REG_CODE_DX  0x2
#define REG_CODE_BX  0x3
#define REG_CODE_SP  0x4
#define REG_CODE_BP  0x5
#define REG_CODE_SI  0x6
#define REG_CODE_DI  0x7

#define REG_CODE_64  0x8

#define REG_CODE_R8  0x8
#define REG_CODE_R9  0x9
#define REG_CODE_R10 0xA
#define REG_CODE_R11 0xB
#define REG_CODE_R12 0xC
#define REG_CODE_R13 0xD
#define REG_CODE_R14 0xE
#define REG_CODE_R15 0xF
#define REG_CODE_SPL 0x10
#define REG_CODE_BPL 0x11
#define REG_CODE_SIL 0x12
#define REG_CODE_DIL 0x13
#define REG_CODE_IP  0x14

#define SREG_CODE_CS 0x0
#define SREG_CODE_DS 0x1
#define SREG_CODE_ES 0x2
#define SREG_CODE_SS 0x3
#define SREG_CODE_FS 0x4
#define SREG_CODE_GS 0x5

#define FREG_CODE_ST0 0x0
#define FREG_CODE_ST1 0x1
#define FREG_CODE_ST2 0x2
#define FREG_CODE_ST3 0x3
#define FREG_CODE_ST4 0x4
#define FREG_CODE_ST5 0x5
#define FREG_CODE_ST6 0x6
#define FREG_CODE_ST7 0x7

#define CREG_CODE_CR0 0x0
#define CREG_CODE_CR1 0x1
#define CREG_CODE_CR2 0x2
#define CREG_CODE_CR3 0x3
#define CREG_CODE_CR4 0x4
#define CREG_CODE_CR5 0x5
#define CREG_CODE_CR6 0x6
#define CREG_CODE_CR7 0x7

#define DREG_CODE_DR0 0x0
#define DREG_CODE_DR1 0x1
#define DREG_CODE_DR2 0x2
#define DREG_CODE_DR3 0x3
#define DREG_CODE_DR4 0x4
#define DREG_CODE_DR5 0x5
#define DREG_CODE_DR6 0x6
#define DREG_CODE_DR7 0x7

#define MREG_CODE_MM0 0x0
#define MREG_CODE_MM1 0x1
#define MREG_CODE_MM2 0x2
#define MREG_CODE_MM3 0x3
#define MREG_CODE_MM4 0x4
#define MREG_CODE_MM5 0x5
#define MREG_CODE_MM6 0x6
#define MREG_CODE_MM7 0x7

#define XREG_CODE_XMM0  0x0
#define XREG_CODE_XMM1  0x1
#define XREG_CODE_XMM2  0x2
#define XREG_CODE_XMM3  0x3
#define XREG_CODE_XMM4  0x4
#define XREG_CODE_XMM5  0x5
#define XREG_CODE_XMM6  0x6
#define XREG_CODE_XMM7  0x7

#define XREG_CODE_XMM8  0x8
#define XREG_CODE_XMM9  0x9
#define XREG_CODE_XMM10 0xA
#define XREG_CODE_XMM11 0xB
#define XREG_CODE_XMM12 0xC
#define XREG_CODE_XMM13 0xD
#define XREG_CODE_XMM14 0xE
#define XREG_CODE_XMM15 0xF

#pragma pack(push, 0x4)
struct OPERAND
{
	uint8_t flags;
	uint16_t size; //Fuck... I need 16_t only for 'stx' size qualifier.

	union
	{
		struct REG
		{
			uint8_t code;
			uint8_t type;
		} reg;

		struct IMM
		{
			uint8_t size;
			uint8_t offset;
			union
			{
				uint8_t  imm8;
				uint16_t imm16;
				uint32_t imm32;
				uint64_t imm64;
			};
		} imm;

		union FAR_ADDR
		{
			struct FAR_ADDR32
			{
				uint16_t offset;
				uint16_t seg;
			} far_addr32;

			struct FAR_ADDR48
			{
				uint32_t offset;
				uint16_t seg;
			} far_addr48;
		} far_addr;

		struct ADDR
		{
			uint8_t seg;
			uint8_t mod;
			uint8_t base;
			uint8_t index;
			uint8_t scale;
		} addr;
	} value;
};
#pragma pack (pop)

/************************
* Defines and structs for
* instruction.
*************************
*/
#define	INSTR_FLAG_MODRM              0x1
#define	INSTR_FLAG_SIB                0x2
#define	INSTR_FLAG_D64                0x4
#define	INSTR_FLAG_F64                0x8
#define	INSTR_FLAG_SUPERFLUOUS_PREFIX 0x10

//Segment override prefixes
#define	INSTR_PREFIX_CS 0x1
#define	INSTR_PREFIX_DS 0x2
#define	INSTR_PREFIX_ES 0x4
#define	INSTR_PREFIX_SS 0x8
#define	INSTR_PREFIX_FS 0x10
#define	INSTR_PREFIX_GS 0x20
//Segment prefixes mask
#define INSTR_PREFIX_SEG_MASK  0x3F
//Repeat prefixes
#define	INSTR_PREFIX_REPZ      0x40
#define	INSTR_PREFIX_REPNZ     0x80
//Repeat prefixes mask
#define INSTR_PREFIX_REP_MASK  0xC0
//Size override prefixes
#define	INSTR_PREFIX_OPSIZE    0x100
#define	INSTR_PREFIX_ADDRSIZE  0x200
#define	INSTR_PREFIX_REX       0x400
//Operand size prefixes mask
#define INSTR_PREFIX_SIZE_MASK 0x300
//LOCK prefix
#define	INSTR_PREFIX_LOCK      0x800

//REX bit fields
#define PREFIX_REX_W 0x8
#define PREFIX_REX_R 0x4
#define PREFIX_REX_X 0x2
#define PREFIX_REX_B 0x1

#define MAX_MNEMONIC_LEN_A  0xC
#define MAX_MNEMONIC_LEN_W  0x18

#define MAX_MNEMONIC_LEN    0xC

#define MAX_INSTRUCTION_LEN 0x10

#define ADDR_SIZE_16 0x2
#define ADDR_SIZE_32 0x4
#define ADDR_SIZE_64 0x8

#define EFLAG_C 0x1
#define EFLAG_P 0x2
#define EFLAG_A 0x4
#define EFLAG_Z 0x8
#define EFLAG_S 0x10
#define EFLAG_I 0x20
#define EFLAG_D 0x40
#define EFLAG_O 0x80

#pragma pack(push, 0x4)
struct DISPLACEMENT
{
	uint8_t size;
	uint8_t offset;
	union VALUE
	{
		int16_t d16;
		int32_t d32;
		int64_t d64;
	} value;
};

struct INSTRUCTION
{
	uint64_t groups;
	uint16_t id;
	uint16_t flags;
	uint16_t prefixes;
	uint8_t  opcode_offset;

	struct OPERAND ops[3];
	struct DISPLACEMENT disp;

	uint8_t addrsize;
	uint8_t modrm;
	uint8_t sib;
	uint8_t rex;

	uint8_t tested_flags;
	uint8_t modified_flags;
	uint8_t set_flags;
	uint8_t cleared_flags;
	uint8_t undefined_flags;

	char mnemonic[MAX_MNEMONIC_LEN];
};

struct PARAMS
{
	uint8_t *sf_prefixes;
	int sf_prefixes_len;
	uint8_t arch;
	uint8_t mode;
};
#pragma pack(pop)

/******************
* Instructions' IDs
*******************
*/
#define ID_NULL        0x0
#define ID_SWITCH      0x1
#define ID_ADD         0x2
#define ID_PUSH        0x3
#define ID_POP         0x4
#define ID_OR          0x5
#define ID_ADC         0x6
#define ID_SBB         0x7
#define ID_AND         0x8
#define ID_DAA         0x9
#define ID_SUB         0xA
#define ID_DAS         0xB
#define ID_XOR         0xC
#define ID_AAA         0xD
#define ID_CMP         0xE
#define ID_AAS         0xF
#define ID_INC         0x10
#define ID_DEC         0x11
#define ID_PUSHA       0x12
#define ID_POPA        0x13
#define ID_BOUND       0x14
#define ID_ARPL        0x15
#define ID_ALTER       0x16
#define ID_IMUL        0x17
#define ID_INS         0x18
#define ID_OUTS        0x19
#define ID_JO          0x1A
#define ID_JNO         0x1B
#define ID_JB          0x1C
#define ID_JAE         0x1D
#define ID_JZ          0x1E
#define ID_JNZ         0x1F
#define ID_JBE         0x20
#define ID_JA          0x21
#define ID_JS          0x22
#define ID_JNS         0x23
#define ID_JP          0x24
#define ID_JNP         0x25
#define ID_JL          0x26
#define ID_JGE         0x27
#define ID_JLE         0x28
#define ID_JG          0x29
#define ID_TEST        0x2A
#define ID_XCHG        0x2B
#define ID_MOV         0x2C
#define ID_LEA         0x2D
#define ID_NOP         0x2E
#define ID_CBW         0x2F
#define ID_CWD         0x30
#define ID_CALLF       0x31
#define ID_FWAIT       0x32
#define ID_PUSHF       0x33
#define ID_POPF        0x34
#define ID_SAHF        0x35
#define ID_LAHF        0x36
#define ID_MOVS        0x37
#define ID_CMPS        0x38
#define ID_STOS        0x39
#define ID_LODS        0x3A
#define ID_SCAS        0x3B
#define ID_ROL         0x3C
#define ID_ROR         0x3D
#define ID_RCL         0x3E
#define ID_RCR         0x3F
#define ID_SHL         0x40
#define ID_SHR         0x41
#define ID_SAL         0x42
#define ID_SAR         0x43
#define ID_RETN        0x44
#define ID_LES         0x45
#define ID_LDS         0x46
#define ID_ENTER       0x47
#define ID_LEAVE       0x48
#define ID_RETF        0x49
#define ID_INT         0x4A
#define ID_INTO        0x4B
#define ID_IRET        0x4C
#define ID_AAM         0x4D
#define ID_AAD         0x4E
#define ID_SETALC      0x4F
#define ID_XLAT        0x50
#define ID_LOOPNZ      0x51
#define ID_LOOPZ       0x52
#define ID_LOOP        0x53
#define ID_JCXZ        0x54
#define ID_IN          0x55
#define ID_OUT         0x56
#define ID_CALL        0x57
#define ID_JMP         0x58
#define ID_JMPF        0x59
#define ID_LOCK        0x5A
#define ID_ICEBP       0x5B
#define ID_REPNZ       0x5C
#define ID_REPZ        0x5D
#define ID_HLT         0x5E
#define ID_CMC         0x5F
#define ID_NOT         0x60
#define ID_NEG         0x61
#define ID_MUL         0x62
#define ID_DIV         0x63
#define ID_IDIV        0x64
#define ID_CLC         0x65
#define ID_STC         0x66
#define ID_CLI         0x67
#define ID_STI         0x68
#define ID_CLD         0x69
#define ID_STD         0x6A
#define ID_LAR         0x6B
#define ID_LSL         0x6C
#define ID_LOADALL     0x6D
#define ID_CLTS        0x6E
#define ID_INVD        0x6F
#define ID_WBINVD      0x70
#define ID_UD2         0x71
#define ID_MOVUPS      0x72
#define ID_MOVLPS      0x73
#define ID_UNPCKLPS    0x74
#define ID_UNPCKHPS    0x75
#define ID_MOVHPS      0x76
#define ID_PREFETCHNTA 0x77
#define ID_PREFETCHT0  0x78
#define ID_PREFETCHT1  0x79
#define ID_PREFETCHT2  0x7A
#define ID_MOVAPS      0x7B
#define ID_CVTPI2PS    0x7C
#define ID_MOVNTPS     0x7D
#define ID_CVTTPS2PI   0x7E
#define ID_CVTPS2PI    0x7F
#define ID_UCOMISS     0x80
#define ID_COMISS      0x81
#define ID_WRMSR       0x82
#define ID_RDTSC       0x83
#define ID_RDMSR       0x84
#define ID_RDPMC       0x85
#define ID_SYSENTER    0x86
#define ID_SYSEXIT     0x87
#define ID_PABSD       0x88
#define ID_PALIGNR     0x89
#define ID_CMOVO       0x8A
#define ID_CMOVNO      0x8B
#define ID_CMOVB       0x8C
#define ID_CMOVAE      0x8D
#define ID_CMOVZ       0x8E
#define ID_CMOVNZ      0x8F
#define ID_CMOVBE      0x90
#define ID_CMOVA       0x91
#define ID_CMOVS       0x92
#define ID_CMOVNS      0x93
#define ID_CMOVP       0x94
#define ID_CMOVNP      0x95
#define ID_CMOVL       0x96
#define ID_CMOVGE      0x97
#define ID_CMOVLE      0x98
#define ID_CMOVG       0x99
#define ID_MOVMSKPS    0x9A
#define ID_SQRTPS      0x9B
#define ID_RSQRTPS     0x9C
#define ID_RCPPS       0x9D
#define ID_ANDPS       0x9E
#define ID_ANDNPS      0x9F
#define ID_ORPS        0xA0
#define ID_XORPS       0xA1
#define ID_ADDPS       0xA2
#define ID_MULPS       0xA3
#define ID_CVTPS2PD    0xA4
#define ID_CVTDQ2PS    0xA5
#define ID_SUBPS       0xA6
#define ID_MINPS       0xA7
#define ID_DIVPS       0xA8
#define ID_MAXPS       0xA9
#define ID_PUNPCKLBW   0xAA
#define ID_PUNPCKLWD   0xAB
#define ID_PUNPCKLDQ   0xAC
#define ID_PACKSSWB    0xAD
#define ID_PCMPGTB     0xAE
#define ID_PCMPGTW     0xAF
#define ID_PCMPGTD     0xB0
#define ID_PACKUSWB    0xB1
#define ID_PUNPCKHBW   0xB2
#define ID_PUNPCKHWD   0xB3
#define ID_PUNPCKHDQ   0xB4
#define ID_PACKSSDW    0xB5
#define ID_MOVD        0xB6
#define ID_MOVQ        0xB7
#define ID_PSHUFW      0xB8
#define ID_PSRLW       0xB9
#define ID_PSRAW       0xBA
#define ID_PSLLW       0xBB
#define ID_PSRLD       0xBC
#define ID_PSRAD       0xBD
#define ID_PSLLD       0xBE
#define ID_PSRLDQ      0xBF
#define ID_PCMPEQB     0xC0
#define ID_PCMPEQW     0xC1
#define ID_PCMPEQD     0xC2
#define ID_EMMS        0xC3
#define ID_SETO        0xC4
#define ID_SETNO       0xC5
#define ID_SETB        0xC6
#define ID_SETAE       0xC7
#define ID_SETZ        0xC8
#define ID_SETNZ       0xC9
#define ID_SETBE       0xCA
#define ID_SETA        0xCB
#define ID_SETS        0xCC
#define ID_SETNS       0xCD
#define ID_SETP        0xCE
#define ID_SETNP       0xCF
#define ID_SETL        0xD0
#define ID_SETGE       0xD1
#define ID_SETLE       0xD2
#define ID_SETG        0xD3
#define ID_CPUID       0xD4
#define ID_BT          0xD5
#define ID_SHLD        0xD6
#define ID_RSM         0xD7
#define ID_BTS         0xD8
#define ID_SHRD        0xD9
#define ID_FXSAVE      0xDA
#define ID_FXRSTOR     0xDB
#define ID_LDMXCSR     0xDC
#define ID_STMXCSR     0xDD
#define ID_LFENCE      0xDE
#define ID_MFENCE      0xDF
#define ID_SFENCE      0xE0
#define ID_CMPXCHG     0xE1
#define ID_LSS         0xE2
#define ID_BTR         0xE3
#define ID_LFS         0xE4
#define ID_LGS         0xE5
#define ID_MOVZX       0xE6
#define ID_JMPE        0xE7
#define ID_UD          0xE8
#define ID_BTC         0xE9
#define ID_BSF         0xEA
#define ID_BSR         0xEB
#define ID_MOVSX       0xEC
#define ID_XADD        0xED
#define ID_CMPPS       0xEE
#define ID_MOVNTI      0xEF
#define ID_PINSRW      0xF0
#define ID_PEXTRW      0xF1
#define ID_SHUFPS      0xF2
#define ID_CMPXCHG8B   0xF3
#define ID_BSWAP       0xF4
#define ID_PSRLQ       0xF5
#define ID_PADDQ       0xF6
#define ID_PMULLW      0xF7
#define ID_PMOVMSKB    0xF8
#define ID_PSUBUSB     0xF9
#define ID_PSUBUSW     0xFA
#define ID_PMINUB      0xFB
#define ID_PAND        0xFC
#define ID_PADDUSB     0xFD
#define ID_PADDUSW     0xFE
#define ID_PMAXUB      0xFF
#define ID_PANDN       0x100
#define ID_PAVGB       0x101
#define ID_PAVGW       0x102
#define ID_PMULHUW     0x103
#define ID_PMULHW      0x104
#define ID_MOVNTQ      0x105
#define ID_PSUBSB      0x106
#define ID_PSUBSW      0x107
#define ID_PMINSW      0x108
#define ID_POR         0x109
#define ID_PADDSB      0x10A
#define ID_PADDSW      0x10B
#define ID_PMAXSW      0x10C
#define ID_PXOR        0x10D
#define ID_PSLLQ       0x10E
#define ID_PMULUDQ     0x10F
#define ID_PMADDWD     0x110
#define ID_PSADBW      0x111
#define ID_MASKMOVQ    0x112
#define ID_PSUBB       0x113
#define ID_PSUBW       0x114
#define ID_PSUBD       0x115
#define ID_PSUBQ       0x116
#define ID_PADDB       0x117
#define ID_PADDW       0x118
#define ID_PADDD       0x119
#define ID_MOVUPD      0x11A
#define ID_MOVLPD      0x11B
#define ID_UNPCKLPD    0x11C
#define ID_UNPCKHPD    0x11D
#define ID_MOVHPD      0x11E
#define ID_MOVAPD      0x11F
#define ID_CVTPI2PD    0x120
#define ID_MOVNTPD     0x121
#define ID_CVTTPD2PI   0x122
#define ID_CVTPD2PI    0x123
#define ID_UCOMISD     0x124
#define ID_COMISD      0x125
#define ID_PSHUFB      0x126
#define ID_PHADDW      0x127
#define ID_PHADDD      0x128
#define ID_PHADDSW     0x129
#define ID_PMADDUBSW   0x12A
#define ID_PHSUBW      0x12B
#define ID_PHSUBD      0x12C
#define ID_PHSUBSW     0x12D
#define ID_PSIGNB      0x12E
#define ID_PSIGNW      0x12F
#define ID_PSIGND      0x130
#define ID_PMULHRSW    0x131
#define ID_PABSB       0x132
#define ID_PABSW       0x133
#define ID_MOVMSKPD    0x134
#define ID_SQRTPD      0x135
#define ID_ANDPD       0x136
#define ID_ANDNPD      0x137
#define ID_ORPD        0x138
#define ID_XORPD       0x139
#define ID_ADDPD       0x13A
#define ID_MULPD       0x13B
#define ID_CVTPD2PS    0x13C
#define ID_CVTPS2DQ    0x13D
#define ID_SUBPD       0x13E
#define ID_MINPD       0x13F
#define ID_DIVPD       0x140
#define ID_MAXPD       0x141
#define ID_PUNPCKLQDQ  0x142
#define ID_PUNPCKHQDQ  0x143
#define ID_MOVDQA      0x144
#define ID_PSHUFD      0x145
#define ID_PSLLDQ      0x146
#define ID_HADDPD      0x147
#define ID_HSUBPD      0x148
#define ID_CMPPD       0x149
#define ID_SHUFPD      0x14A
#define ID_ADDSUBPD    0x14B
#define ID_CVTTPD2DQ   0x14C
#define ID_MOVNTDQ     0x14D
#define ID_MASKMOVDQU  0x14E
#define ID_MOVSD       0x14F
#define ID_MOVDDUP     0x150
#define ID_CVTSI2SD    0x151
#define ID_CVTTSD2SI   0x152
#define ID_CVTSD2SI    0x153
#define ID_SQRTSD      0x154
#define ID_ADDSD       0x155
#define ID_MULSD       0x156
#define ID_CVTSD2SS    0x157
#define ID_SUBSD       0x158
#define ID_MINSD       0x159
#define ID_DIVSD       0x15A
#define ID_MAXSD       0x15B
#define ID_PSHUFLW     0x15C
#define ID_HADDPS      0x15D
#define ID_HSUBPS      0x15E
#define ID_CMPSD       0x15F
#define ID_ADDSUBPS    0x160
#define ID_MOVDQ2Q     0x161
#define ID_CVTPD2DQ    0x162
#define ID_LDDQU       0x163
#define ID_MOVSS       0x164
#define ID_MOVSLDUP    0x165
#define ID_MOVSHDUP    0x166
#define ID_CVTSI2SS    0x167
#define ID_CVTTSS2SI   0x168
#define ID_CVTSS2SI    0x169
#define ID_SQRTSS      0x16A
#define ID_RSQRTSS     0x16B
#define ID_RCPSS       0x16C
#define ID_ADDSS       0x16D
#define ID_MULSS       0x16E
#define ID_CVTSS2SD    0x16F
#define ID_CVTTPS2DQ   0x170
#define ID_SUBSS       0x171
#define ID_MINSS       0x172
#define ID_DIVSS       0x173
#define ID_MAXSS       0x174
#define ID_MOVDQU      0x175
#define ID_PSHUFHW     0x176
#define ID_CMPSS       0x177
#define ID_MOVQ2DQ     0x178
#define ID_CVTDQ2PD    0x179

#define ID_0F          0x17A
#define ID_ES          0x17B
#define ID_CS          0x17C
#define ID_DS          0x17D
#define ID_FS          0x17E
#define ID_GS          0x17F
#define ID_SS          0x180
#define ID_66          0x181
#define ID_67          0x182
#define ID_CLFLUSH     0x186
#define ID_MOVHLPS     0x187
#define ID_PAUSE       0x188
#define ID_MOVSXD      0x189
#define ID_SYSCALL     0x18A
#define ID_SYSRET      0x18B
#define ID_GETSEC      0x18C
#define ID_VMREAD      0x18D
#define ID_VMWRITE     0x18E
#define ID_XSAVE       0x18F
#define ID_VMPTRLD     0x190
#define ID_VMPTRST     0x191
#define ID_VMCLEAR     0x192
#define ID_POPCNT      0x193
#define ID_VMXON       0x194
#define ID_INVEPT      0x195
#define ID_INVVPID     0x196
#define ID_MOVBE       0x197
#define ID_PBLENDVB    0x198
#define ID_BLENDVPS    0x199
#define ID_BLENDVPD    0x19A
#define ID_PTEST       0x19B
#define ID_PMOVSXBW    0x19C
#define ID_PMOVSXBD    0x19D
#define ID_PMOVSXBQ    0x19E
#define ID_PMOVSXWD    0x19F
#define ID_PMOVSXWQ    0x1A0
#define ID_PMOVSXDQ    0x1A1
#define ID_PMULDQ      0x1A2
#define ID_PCMPEQQ     0x1A3
#define ID_MOVNTDQA    0x1A4
#define ID_PACKUSDW    0x1A5
#define ID_PMOVZXBW    0x1A6
#define ID_PMOVZXBD    0x1A7
#define ID_PMOVZXBQ    0x1A8
#define ID_PMOVZXWD    0x1A9
#define ID_PMOVZXWQ    0x1AA
#define ID_PMOVZXDQ    0x1AB
#define ID_PCMPGTQ     0x1AC
#define ID_PMINSB      0x1AD
#define ID_PMINSD      0x1AE
#define ID_PMINUW      0x1AF
#define ID_PMINUD      0x1B0
#define ID_PMAXSB      0x1B1
#define ID_PMAXSD      0x1B2
#define ID_PMAXUW      0x1B3
#define ID_PMAXUD      0x1B4
#define ID_PMULLD      0x1B5
#define ID_PHMINPOSUW  0x1B6
#define ID_CRC32       0x1B7
#define ID_ROUNDPD     0x1B8
#define ID_ROUNDSS     0x1B9
#define ID_ROUNDSD     0x1BA
#define ID_BLENDPS     0x1BB
#define ID_BLENDPD     0x1BC
#define ID_PBLENDW     0x1BD
#define ID_PEXTRB      0x1BE
#define ID_PEXTRQ      0x1BF
#define ID_EXTRACTPS   0x1C0
#define ID_PINSRB      0x1C1
#define ID_INSERTPS    0x1C2
#define ID_PINSRQ      0x1C3
#define ID_DPPS        0x1C4
#define ID_DPPD        0x1C5
#define ID_MPSADBW     0x1C6
#define ID_PCMPESTRM   0x1C7
#define ID_PCMPESTRI   0x1C8
#define ID_PCMPISTRM   0x1C9
#define ID_PCMPISTRI   0x1CA
#define ID_ROUNDPS     0x1CB

/*********************
* Instructions' groups
**********************
*/
#define GRP_NULL     0x0
#define GRP_SWITCH   0x1
#define GRP_GEN      0x2
#define GRP_SSE1     0x4
#define GRP_ARITH    0x8
#define GRP_BINARY   0x10
#define GRP_STACK    0x20
#define GRP_SEGREG   0x40
#define GRP_LOGICAL  0x80
#define GRP_PREFIX   0x100
#define GRP_BRANCH   0x200
#define GRP_COND     0x400
#define GRP_DECIMAL  0x800
#define GRP_BREAK    0x1000
#define GRP_SYSTEM   0x2000
#define GRP_INOUT    0x4000
#define GRP_STRING   0x8000
#define GRP_DATAMOV  0x10000
#define GRP_CONTROL  0x20000
#define GRP_CONVER   0x40000
#define GRP_X87FPU   0x80000
#define GRP_FLGCTRL  0x100000
#define GRP_SHFTROT  0x200000
#define GRP_SIMDFP   0x400000
#define GRP_SHUNPCK  0x800000
#define GRP_FETCH    0x1000000
#define GRP_CACHECT  0x2000000
#define GRP_COMPAR   0x4000000
#define GRP_TRANS    0x8000000
#define GRP_SIMDINT  0x10000000
#define GRP_PCKSCLR  0x20000000
#define GRP_PCKSP    0x40000000
#define GRP_UNPACK   0x80000000
#define GRP_SHIFT    0x100000000L
#define GRP_BIT      0x200000000L
#define GRP_SM       0x400000000L
#define GRP_MXCSRSM  0x800000000L
#define GRP_ORDER    0x1000000000L
#define GRP_SSE2     0x2000000000L
#define GRP_SSSE3    0x4000000000L
#define GRP_MMX      0x8000000000L
#define GRP_SSE3     0x10000000000L

#define GRP_SMX    0x20000000000L
#define GRP_VMX    0x40000000000L
#define GRP_SSE41  0x80000000000L
#define GRP_SSE42  0x100000000000L
#define GRP_STRTXT 0x200000000000L

/***************************
* Instructions' architecture
****************************
*/
#define ARCH_COMMON 0x1
#define ARCH_INTEL  0x2
#define ARCH_AMD    0x4

/******************
* Disassemble modes
*******************
*/
#define DISASSEMBLE_MODE_16 0x1
#define DISASSEMBLE_MODE_32 0x2
#define DISASSEMBLE_MODE_64 0x4

/*******
* Errors
********
*/
#define ERR_BADCODE      0x1
#define ERR_TOO_LONG     0x2
#define ERR_NON_LOCKABLE 0x3
#define ERR_RM_REG       0x4
#define ERR_RM_MEM       0x5
#define ERR_16_32_ONLY   0x6
#define ERR_64_ONLY      0x7
#define ERR_REX_NOOPCD   0x8
#define ERR_ANOT_ARCH    0x9
#define ERR_INTERNAL     0xA

#define IS_OK(res) (!((res) & ~0xFF))
#define GET_ERR(e) (((e) >> 0x8) & 0xFF)

/*******************
* Dumping parameters
*  and routines
********************
*/
#define OPTION_IMM_HEX   0x1
#define OPTION_IMM_UHEX  0x2
#define OPTION_IMM_DEC   0x4
#define OPTION_IMM_UDEC  0x8
#define OPTION_IMM_MASK  0xF

#define OPTION_DISP_HEX  0x10
#define OPTION_DISP_UHEX 0x20
#define OPTION_DISP_DEC  0x40
#define OPTION_DISP_UDEC 0x80
#define OPTION_DISP_MASK 0xF0

#ifdef __cplusplus
extern "C" {
#endif

int dump(struct INSTRUCTION *instr, char *buff, int bufflen, int options);
int dump_instruction(struct INSTRUCTION *instr, char *buff, int bufflen);
int dump_operand(struct INSTRUCTION *instr, int op_index, int options, char *buff, int bufflen);
int dump_dbg(struct INSTRUCTION *instr, uint8_t *sf_prefixes, char *buff, unsigned int len);

/***************************
* Disassembler's entry point
****************************
*/
unsigned int disassemble(uint8_t *offset, struct INSTRUCTION *instr, struct PARAMS *params);

#ifdef __cplusplus
}
#endif

#endif // DISASM_H_
