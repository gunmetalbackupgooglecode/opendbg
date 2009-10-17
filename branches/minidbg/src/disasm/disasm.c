#undef UNICODE
#undef _UNICODE

#include <string.h>
#include "pstdint.h"
#include <stdio.h>

#include "disasm.h"
#include "tables.h"
#include "tables.inc"

#define ERR_SHIFT   0x8
#define MAKE_ERR(v, e) (v | (e << ERR_SHIFT))
#define GET_LEN(v) ((v) & 0xFF)

/*************************************
* Postprocessing functions prototypes.
**************************************
*/
uint32_t post_proc_arpl_movsxd(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, uint8_t mode);
uint32_t post_proc_nop_pause(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, uint8_t mode);
uint32_t post_proc_multinop(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, uint8_t mode);
uint32_t post_proc_cmpxchg8b(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, uint8_t mode);

//Array of potsprocessning handlers.
uint32_t (*postprocs[])(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, uint8_t mode) = 
{
	post_proc_arpl_movsxd,
	post_proc_nop_pause,
	post_proc_multinop,
	post_proc_cmpxchg8b
};

/******************************************************
* Operand's type qualifers (TQ_*) handlers' prototypes.
*******************************************************
*/
uint32_t opt_1(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_3(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_A(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_C(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_D(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_E(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_G(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_H(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_I(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_J(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_M(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_N(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_O(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_P(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_Q(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_R(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_S(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_T(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_U(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_V(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_W(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_X(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_Y(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_Z(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_rAX(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_rCX(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_rDX(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_rBX(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_rSP(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_rBP(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_rSI(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_rDI(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_fST0(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_fES(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_fEST(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_CS(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_DS(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_SS(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_ES(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_FS(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t opt_GS(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t pref_CS_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t pref_DS_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t pref_SS_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t pref_ES_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t pref_FS_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t pref_GS_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t pref_OPSIZE_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t pref_ADDRSIZE_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t pref_REPZ_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t pref_REPNZ_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
uint32_t pref_LOCK_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);

//Array of type qualifiers' handlers.
uint32_t (*opt_handlers[])(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode) = 
{
	opt_1,
	opt_3,
	opt_A,
	opt_C,
	opt_D,
	opt_E,
	opt_G,
	opt_H,
	opt_I,
	opt_J,
	opt_M,
	opt_N,
	opt_O,
	opt_P,
	opt_Q,
	opt_R,
	opt_S,
	opt_T,
	opt_U,
	opt_V,
	opt_W,
	opt_X,
	opt_Y,
	opt_Z,
	opt_rAX,
	opt_rCX,
	opt_rDX,
	opt_rBX,
	opt_rSP,
	opt_rBP,
	opt_rSI,
	opt_rDI,
	opt_fES,
	opt_fEST,
	opt_fST0,
	opt_CS,
	opt_DS,
	opt_ES,
	opt_SS,
	opt_FS,
	opt_GS,	
	pref_CS_set,
	pref_DS_set,
	pref_ES_set,
	pref_SS_set,
	pref_FS_set,
	pref_GS_set,
	pref_OPSIZE_set,
	pref_ADDRSIZE_set,
	pref_REPZ_set,
	pref_REPNZ_set,
	pref_LOCK_set
};

/*******************************************************
* Operand's size qualifiers (SQ_*) handlers' prototypes.
********************************************************
*/
void ops_a(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_b(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_bcd(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_bdqp(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_bs(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_bss(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_d(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_ddq(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_di(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_dq(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_dqp(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_dr(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_ds(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_e(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_er(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_p(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_pd(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_pi(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_ps(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_psq(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_ptp(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_q(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_qdq(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_qi(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_sd(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_sr(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_ss(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_st(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_stx(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_v(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_vds(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_vqp(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_vs(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_w(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_wdq(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_wdqp(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_wi(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_wv(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);
void ops_wvqp(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize);

//Array of type qualifiers' handlers.
void (*ops_handlers[])(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize) = 
{
	ops_a,
	ops_b,
	ops_bcd,
	ops_bdqp,
	ops_bs,
	ops_bss,
	ops_d,
	ops_ddq,
	ops_di,
	ops_dq,
	ops_dr,
	ops_ds,	
	ops_dqp,
	ops_e,
	ops_er,
	ops_p,
	ops_pd,
	ops_pi,
	ops_ps,
	ops_psq,
	ops_ptp,
	ops_q,
	ops_qdq,
	ops_qi,
	ops_sd,
	ops_sr,
	ops_ss,
	ops_st,
	ops_stx,
	ops_v,
	ops_vds,
	ops_vqp,
	ops_vs,
	ops_w,
	ops_wdq,
	ops_wdqp,
	ops_wi,
	ops_wv,
	ops_wvqp
};
//Table for bulding 16bit addresses in my representation.
static struct ADDR addrs_16bit[] =
{
	  //seg         mod                                           base         index        scale
	{ SREG_CODE_DS, ADDR_MOD_BASE | ADDR_MOD_IDX,                 REG_CODE_BX, REG_CODE_SI, 0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE | ADDR_MOD_IDX,                 REG_CODE_BX, REG_CODE_DI, 0x1 },
	{ SREG_CODE_SS, ADDR_MOD_BASE | ADDR_MOD_IDX,                 REG_CODE_BP, REG_CODE_SI, 0x1 },
	{ SREG_CODE_SS, ADDR_MOD_BASE | ADDR_MOD_IDX,                 REG_CODE_BP, REG_CODE_DI, 0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE,                                REG_CODE_SI, 0x0,         0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE,                                REG_CODE_DI, 0x0,         0x1 },
	{ SREG_CODE_DS, ADDR_MOD_DISP,                                0x0,         0x0,         0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE,                                REG_CODE_BX, 0x0,         0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE | ADDR_MOD_IDX | ADDR_MOD_DISP, REG_CODE_BX, REG_CODE_SI, 0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE | ADDR_MOD_IDX | ADDR_MOD_DISP, REG_CODE_BX, REG_CODE_DI, 0x1 },
	{ SREG_CODE_SS, ADDR_MOD_BASE | ADDR_MOD_IDX | ADDR_MOD_DISP, REG_CODE_BP, REG_CODE_SI, 0x1 },
	{ SREG_CODE_SS, ADDR_MOD_BASE | ADDR_MOD_IDX | ADDR_MOD_DISP, REG_CODE_BP, REG_CODE_DI, 0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE | ADDR_MOD_DISP,                REG_CODE_SI, 0x0,         0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE | ADDR_MOD_DISP,                REG_CODE_DI, 0x0,         0x1 },
	{ SREG_CODE_SS, ADDR_MOD_BASE | ADDR_MOD_DISP,                REG_CODE_BP, 0x0,         0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE | ADDR_MOD_DISP,                REG_CODE_BX, 0x0,         0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE | ADDR_MOD_IDX | ADDR_MOD_DISP, REG_CODE_BX, REG_CODE_SI, 0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE | ADDR_MOD_IDX | ADDR_MOD_DISP, REG_CODE_BX, REG_CODE_DI, 0x1 },
	{ SREG_CODE_SS, ADDR_MOD_BASE | ADDR_MOD_IDX | ADDR_MOD_DISP, REG_CODE_BP, REG_CODE_SI, 0x1 },
	{ SREG_CODE_SS, ADDR_MOD_BASE | ADDR_MOD_IDX | ADDR_MOD_DISP, REG_CODE_BP, REG_CODE_DI, 0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE | ADDR_MOD_DISP,                REG_CODE_SI, 0x0,         0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE | ADDR_MOD_DISP,                REG_CODE_DI, 0x0,         0x1 },
	{ SREG_CODE_SS, ADDR_MOD_BASE | ADDR_MOD_DISP,                REG_CODE_BP, 0x0,         0x1 },
	{ SREG_CODE_DS, ADDR_MOD_BASE | ADDR_MOD_DISP,                REG_CODE_BX, 0x0,         0x1 }
};

/*******************************************
* Handler for clearing superfluous prefixes.
********************************************
*/
static void pref_CS_clear(struct INSTRUCTION *instr);
static void pref_DS_clear(struct INSTRUCTION *instr);
static void pref_SS_clear(struct INSTRUCTION *instr);
static void pref_ES_clear(struct INSTRUCTION *instr);
static void pref_FS_clear(struct INSTRUCTION *instr);
static void pref_GS_clear(struct INSTRUCTION *instr);
static void pref_REPZ_clear(struct INSTRUCTION *instr);
static void pref_REPNZ_clear(struct INSTRUCTION *instr);
static void pref_REX_clear(struct INSTRUCTION *instr);
static void pref_ADDRSIZE_clear(struct INSTRUCTION *instr);
static void pref_OPSIZE_clear(struct INSTRUCTION *instr);

//Array of handlers for clearing superfluous prefixes.
void (*pref_clear_handlers[])(struct INSTRUCTION *instr) = 
{
	pref_CS_clear,
	pref_DS_clear,
	pref_ES_clear,
	pref_SS_clear,
	pref_FS_clear,
	pref_GS_clear,
	pref_REPZ_clear,
	pref_REPNZ_clear,
	pref_OPSIZE_clear,
	pref_ADDRSIZE_clear,
	pref_REX_clear
};

/**********************************
* Some usorted internal prototypes.
***********************************
*/
static void create_reg_operand(struct OPERAND *op, uint8_t type, uint8_t code, uint16_t size);
static void create_xmmreg_operand(struct INSTRUCTION *instr, struct OPERAND *op, uint8_t code, uint16_t size, uint8_t mode);
static void create_genreg_operand(struct INSTRUCTION *instr, struct OPERAND *op, uint8_t code, uint16_t size, uint8_t rex, uint8_t mode);
static void get_seg(struct INSTRUCTION *instr, struct OPERAND *op, uint8_t mode);
static uint8_t get_operand_size(struct INSTRUCTION *instr, uint8_t mode);
static uint8_t get_operand_size_16_32(struct INSTRUCTION *instr, uint8_t mode);
static uint32_t parse_mem_operand(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
static uint32_t parse_rm_operand(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode);
static void movsx(void *value, unsigned int size1, unsigned int size2);

static uint8_t sregs[] =
{
	SREG_CODE_CS,
	SREG_CODE_DS,
	SREG_CODE_ES,
	SREG_CODE_SS,
	SREG_CODE_FS,
	SREG_CODE_GS
};

static uint8_t pref_opcodes[] =
{
	0x2E, //CS
	0x3E, //DS
	0x26, //ES
	0x36, //SS
	0xF2, //REPZ
	0xF3, //REPNZ
	0x65,  //ADDRSIZE
	0x66  //OPSIZE
};

/********************************************
* Operand's type qualifiers' (TQ_*) handlers.
*********************************************
*/
uint32_t opt_1(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	op ->flags |= OPERAND_TYPE_IMM;
	op ->size = OPERAND_SIZE_8;
	op ->value.imm.imm8 = 0x1;

	return 0x0;
}

uint32_t opt_3(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	op ->flags |= OPERAND_TYPE_IMM;
	op ->size = OPERAND_SIZE_8;
	op ->value.imm.imm8 = 0x3;

	return 0;
}

uint32_t opt_A(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	op ->flags |= OPERAND_TYPE_DIR;
	op ->size = opsize ->size;
	memcpy(&(op ->value.far_addr), offset, op ->size);

	return (uint8_t)op ->size;
}

uint32_t opt_C(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_CR, (instr ->modrm >> 0x3) & 0x7, opsize ->size);

	return 0x0;
}

uint32_t opt_D(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_DBG, (instr ->modrm >> 0x3) & 0x7, opsize ->size);

	return 0x0;
}

uint32_t opt_E(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	return parse_rm_operand(origin_offset, offset, instr, op, opsize, mode);
}


uint32_t opt_G(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_genreg_operand(instr, op, (instr ->modrm >> 3) & 0x7, opsize ->size, PREFIX_REX_R, mode);

	return 0x0;
}

uint32_t opt_H(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_genreg_operand(instr, op, instr ->modrm & 0x7, opsize ->size, PREFIX_REX_B, mode);

	return 0x0;
}

uint32_t opt_I(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	op ->flags |= OPERAND_TYPE_IMM;
	op ->size = opsize ->size;
	op ->value.imm.size = (uint8_t)opsize ->size_in_stream;
	op ->value.imm.offset = (uint8_t)(offset - origin_offset);
	memcpy(&(op ->value.imm.imm8), offset, opsize ->size_in_stream);
	//movsx(&(op ->value.imm.imm8), opsize ->size_in_stream, opsize ->size);
	movsx(&(op ->value.imm.imm8), opsize ->size_in_stream, 0x8);

	return (uint8_t)opsize ->size_in_stream;
}

uint32_t opt_J(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	return opt_I(origin_offset, offset, instr, op, opsize, mode);
}

uint32_t opt_M(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res = parse_rm_operand(origin_offset, offset, instr, op, opsize, mode);
	if ((instr ->modrm & 0xC0) == 0xC0)
	{
		res = MAKE_ERR(res, ERR_RM_REG);//error: rm encodes memory.
	}
	return res;
}

uint32_t opt_N(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_MMX, instr ->modrm & 0x7, opsize ->size);

	return 0x0;
}

uint32_t opt_O(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	res = instr ->addrsize;
	op ->flags |= OPERAND_TYPE_MEM;
	op ->size = opsize ->size;
	op ->value.addr.mod = ADDR_MOD_DISP;
	memcpy(&(instr ->disp.value), offset, instr ->addrsize);
	get_seg(instr, op, mode);

	return res;
}

uint32_t opt_P(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_MMX, (instr ->modrm >> 0x3) & 0x7, opsize ->size);

	return 0x0;
}

uint32_t opt_Q(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	res = 0x0;
	if ((instr ->modrm & 0xC0) == 0xC0)
	{
		create_reg_operand(op, REG_TYPE_MMX, instr ->modrm & 0x7, opsize ->size);
	}
	else
	{
		res = parse_mem_operand(origin_offset, offset, instr, op, opsize, mode);
	}

	return res;
}

uint32_t opt_R(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res = parse_rm_operand(origin_offset, offset, instr, op, opsize, mode);
	if ((instr ->modrm & 0xC0) != 0xC0)
	{
		res = MAKE_ERR(res, ERR_RM_MEM);//error: rm encodes memory.
	}
	return res;
}

uint32_t opt_S(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_SEG, (instr ->modrm >> 3) & 0x7, OPERAND_SIZE_16);

	return 0x0;
}

uint32_t opt_T(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_TR, (instr ->modrm >> 0x3) & 0x7, opsize ->size);

	return 0x0;
}

uint32_t opt_U(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_XMM, instr ->modrm & 0x7, opsize ->size);

	return 0x0;
}

uint32_t opt_V(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_xmmreg_operand(instr, op, (instr ->modrm >> 0x3) & 0x7, opsize ->size, mode);

	return 0;
}

uint32_t opt_W(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	if ((instr ->modrm & 0xC0) == 0xC0)
	{
		create_xmmreg_operand(instr, op, instr ->modrm & 0x7, opsize ->size, mode);
		res = 0;
	}
	else
	{
		res = parse_mem_operand(origin_offset, offset, instr, op, opsize, mode);
	}

	return res;
}

uint32_t opt_X(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	res = 0;
	op ->flags |= OPERAND_TYPE_MEM;
	op ->size = opsize ->size;
	op ->value.addr.mod = ADDR_MOD_BASE;
	op ->value.addr.base = REG_CODE_SI;
	get_seg(instr, op, mode);

	return 0x0;
}

uint32_t opt_Y(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	op ->flags |= OPERAND_TYPE_MEM;
	op ->size = opsize ->size;
	if (mode == DISASSEMBLE_MODE_64)
		op ->value.addr.seg = SREG_CODE_CS;
	else
		op ->value.addr.seg = SREG_CODE_ES;
	op ->value.addr.mod = ADDR_MOD_BASE;
	op ->value.addr.base = REG_CODE_DI;

	return 0x0;
}

uint32_t opt_Z(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	//We already consumed opcode, hence we need to look backward.
	create_reg_operand(op, REG_TYPE_GEN, offset[-1] & 0x7, opsize ->size);

	return 0x0;
}

uint32_t opt_rAX(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_genreg_operand(instr, op, REG_CODE_AX, opsize ->size, PREFIX_REX_B, mode);

	return 0x0;
}

uint32_t opt_rCX(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_genreg_operand(instr, op, REG_CODE_CX, opsize ->size, PREFIX_REX_B, mode);

	return 0x0;
}

uint32_t opt_rDX(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_genreg_operand(instr, op, REG_CODE_DX, opsize ->size, PREFIX_REX_B, mode);

	return 0x0;
}

uint32_t opt_rBX(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_genreg_operand(instr, op, REG_CODE_BX, opsize ->size, PREFIX_REX_B, mode);

	return 0x0;
}

uint32_t opt_rSP(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_genreg_operand(instr, op, REG_CODE_SP, opsize ->size, PREFIX_REX_B, mode);

	return 0x0;
}

uint32_t opt_rBP(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_genreg_operand(instr, op, REG_CODE_BP, opsize ->size, PREFIX_REX_B, mode);

	return 0x0;
}

uint32_t opt_rSI(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_genreg_operand(instr, op, REG_CODE_SI, opsize ->size, PREFIX_REX_B, mode);

	return 0x0;
}

uint32_t opt_rDI(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_genreg_operand(instr, op, REG_CODE_DI, opsize ->size, PREFIX_REX_B, mode);

	return 0x0;
}

uint32_t opt_fES(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	if ((instr ->modrm & 0xC0) == 0xC0)
		res = opt_fEST(origin_offset, offset, instr, op, opsize, mode);
	else
		res = opt_M(origin_offset, offset, instr, op, opsize, mode);

	return res;
}

uint32_t opt_fEST(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_FPU, instr ->modrm & 0x7, opsize ->size);

	return 0;
}

uint32_t opt_fST0(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_FPU, FREG_CODE_ST0, opsize ->size);

	return 0;
}

uint32_t opt_CS(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_SEG, SREG_CODE_CS, opsize ->size);

	return 0;
}

uint32_t opt_DS(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_SEG, SREG_CODE_DS, opsize ->size);

	return 0;
}

uint32_t opt_ES(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_SEG, SREG_CODE_ES, opsize ->size);

	return 0;
}

uint32_t opt_SS(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_SEG, SREG_CODE_SS, opsize ->size);

	return 0;
}

uint32_t opt_FS(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_SEG, SREG_CODE_FS, opsize ->size);

	return 0;
}

uint32_t opt_GS(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	create_reg_operand(op, REG_TYPE_SEG, SREG_CODE_GS, opsize ->size);

	return 0;
}

/********************************************
* Operand's size qualifiers' (TQ_*) handlers.
*********************************************
*/
void ops_a(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
}

void ops_b(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_8;
	opsize ->size = OPERAND_SIZE_8;
	opsize ->sign = 0;
}

void ops_bcd(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	ops_er(instr, mode, opsize);
}

void ops_bdqp(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	if ((instr ->modrm & 0xC0) == 0xC0)
	{
		ops_dqp(instr, mode, opsize);
	}
	else
	{
		ops_b(instr, mode, opsize);
	}
}

void ops_bs(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	ops_b(instr, mode, opsize);
	opsize ->sign = 1;
}

void ops_bss(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_8;
	opsize ->size = get_operand_size(instr, mode);
	opsize ->sign = 1;
}

void ops_d(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_32;
	opsize ->size = OPERAND_SIZE_32;
	opsize ->sign = 0;
}

void ops_ddq(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	if ((instr ->modrm & 0xC0) == 0xC0)
	{
		ops_dq(instr, mode, opsize);
	}
	else
	{
		ops_d(instr, mode, opsize);
	}
}

void ops_di(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_32;
	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_dq(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_128;
	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_dqp(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	if (mode == DISASSEMBLE_MODE_64)
	{
		if ( (instr ->flags & INSTR_FLAG_D64) ||
		   	 (instr ->flags & INSTR_FLAG_F64) ||
		   	 ((instr ->prefixes & INSTR_PREFIX_REX) && (instr ->rex & PREFIX_REX_W)) )
		{
			opsize ->size_in_stream = OPERAND_SIZE_64;
			opsize ->size = OPERAND_SIZE_64;
			opsize ->sign = 1;
		}
		else
		{
			ops_d(instr, mode, opsize);
		}
	}
	else
	{
		ops_d(instr, mode, opsize);
	}
}

void ops_dr(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_64;
	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_ds(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	ops_d(instr, mode, opsize);
	opsize ->sign = 1;
}

void ops_e(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	if (get_operand_size_16_32(instr, mode) == OPERAND_SIZE_16)
	{
		opsize ->size_in_stream = OPERAND_SIZE_14;
	}
	else
	{
		opsize ->size_in_stream = OPERAND_SIZE_28;
	}

	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_er(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_80;
	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_p(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = get_operand_size_16_32(instr, mode) + OPERAND_SIZE_16;
	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_pd(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	ops_dq(instr, mode, opsize);
}

void ops_pi(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	ops_q(instr, mode, opsize);
}

void ops_ps(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_128;
	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_psq(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	ops_q(instr, mode, opsize);
}

void ops_ptp(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	if (mode == DISASSEMBLE_MODE_64)
	{
		opsize ->size_in_stream = get_operand_size(instr, mode) + OPERAND_SIZE_16;
		opsize ->size = opsize ->size_in_stream;
		opsize ->sign = 0;
	}
	else
	{
		ops_p(instr, mode, opsize);
	}
}

void ops_q(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_64;
	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_qdq(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	if ((instr ->modrm & 0xC0) == 0xC0)
	{
		ops_dq(instr, mode, opsize);
	}
	else
	{
		ops_q(instr, mode, opsize);
	}
}

void ops_qi(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_64;
	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_sd(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	ops_dq(instr, mode, opsize);
}

void ops_sr(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_32;
	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_ss(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	ops_dq(instr, mode, opsize);
}

void ops_st(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	if (get_operand_size_16_32(instr, mode) == OPERAND_SIZE_16)
	{
		opsize ->size_in_stream = OPERAND_SIZE_94;
	}
	else
	{
		opsize ->size_in_stream = OPERAND_SIZE_108;
	}

	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_stx(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_512;
	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_v(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = get_operand_size_16_32(instr, mode);;
	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_vds(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	if (mode == DISASSEMBLE_MODE_64)
	{
		opsize ->size_in_stream = OPERAND_SIZE_32;
		opsize ->size = OPERAND_SIZE_64;
		opsize ->sign = 1;
	}
	else
	{
		ops_v(instr, mode, opsize);
	}
}

void ops_vqp(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	if (mode == DISASSEMBLE_MODE_64)
	{
		opsize ->size_in_stream = get_operand_size(instr, mode);
		opsize ->size = opsize ->size_in_stream;
		opsize ->sign = 0;
	}
	else
	{
		ops_v(instr, mode, opsize);
	}
}

void ops_vs(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = get_operand_size_16_32(instr, mode);
	opsize ->size = get_operand_size(instr, mode);
	opsize ->sign = 1;
}

void ops_w(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_16;
	opsize ->size = OPERAND_SIZE_16;
	opsize ->sign = 0;
}

void ops_wdq(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	if ((instr ->modrm & 0xC0) == 0xC0)
	{
		ops_dq(instr, mode, opsize);
	}
	else
	{
		ops_w(instr, mode, opsize);
	}
}

void ops_wdqp(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	if ((instr ->modrm & 0xC0) == 0xC0)
	{
		ops_dqp(instr, mode, opsize);
	}
	else
	{
		ops_w(instr, mode, opsize);
	}
}

void ops_wi(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	opsize ->size_in_stream = OPERAND_SIZE_16;
	opsize ->size = opsize ->size_in_stream;
	opsize ->sign = 0;
}

void ops_wv(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	if ((instr ->modrm & 0xC0) == 0xC0)
	{
		ops_v(instr, mode, opsize);
	}
	else
	{
		ops_w(instr, mode, opsize);
	}
}

void ops_wvqp(struct INSTRUCTION *instr, uint8_t mode, struct OPERAND_SIZE *opsize)
{
	if ((instr ->modrm & 0xC0) == 0xC0)
	{
		ops_vqp(instr, mode, opsize);
	}
	else
	{
		ops_w(instr, mode, opsize);
	}
}

uint32_t pref_CS_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	if (instr ->prefixes & INSTR_PREFIX_SEG_MASK)
	{
		instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
		res = 0;
	}
	else
	{
		instr ->prefixes |= INSTR_PREFIX_CS;
		res = 1;
	}

	return res;
}

uint32_t pref_DS_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	if (instr ->prefixes & INSTR_PREFIX_SEG_MASK)
	{
		instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
		res = 0;
	}
	else
	{
		instr ->prefixes |= INSTR_PREFIX_DS;
		res = 1;
	}

	return res;
}

uint32_t pref_ES_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	if (instr ->prefixes & INSTR_PREFIX_SEG_MASK)
	{
		instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
		res = 0;
	}
	else
	{
		instr ->prefixes |= INSTR_PREFIX_ES;
		res = 1;
	}
	
	return res;
}

uint32_t pref_SS_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	if (instr ->prefixes & INSTR_PREFIX_SEG_MASK)
	{
		instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
		res = 0;
	}
	else
	{
		instr ->prefixes |= INSTR_PREFIX_SS;
		res = 1;
	}

	return res;
}

uint32_t pref_FS_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	if (instr ->prefixes & INSTR_PREFIX_SEG_MASK)
	{
		instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
		res = 0;
	}
	else
	{
		instr ->prefixes |= INSTR_PREFIX_FS;
		res = 1;
	}

	return res;
}

uint32_t pref_GS_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	if (instr ->prefixes & INSTR_PREFIX_SEG_MASK)
	{
		instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
		res = 0;
	}
	else
	{
		instr ->prefixes |= INSTR_PREFIX_GS;
		res = 1;
	}

	return res;
}

uint32_t pref_OPSIZE_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	if (instr ->prefixes & INSTR_PREFIX_SIZE_MASK)
	{
		instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
		res = 0;
	}
	else
	{
		instr ->prefixes |= INSTR_PREFIX_OPSIZE;
		res = 1;
	}

	return res;
}

uint32_t pref_ADDRSIZE_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	if (instr ->prefixes & INSTR_PREFIX_SIZE_MASK)
	{
		instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
		res = 0;
	}
	else
	{
		instr ->prefixes |= INSTR_PREFIX_ADDRSIZE;
		res = 1;
	}

	return res;
}

uint32_t pref_REPZ_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	if (instr ->prefixes & INSTR_PREFIX_REP_MASK)
	{
		instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
		res = 0;
	}
	else
	{
		instr ->prefixes |= INSTR_PREFIX_REPZ;
		res = 1;
	}

	return res;
}

uint32_t pref_REPNZ_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	if (instr ->prefixes & INSTR_PREFIX_REP_MASK)
	{
		instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
		res = 0;
	}
	else
	{
		instr ->prefixes |= INSTR_PREFIX_REPNZ;
		res = 1;
	}

	return res;
}

uint32_t pref_LOCK_set(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t res;

	if (instr ->prefixes & INSTR_PREFIX_LOCK)
	{
		instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
		res = 0;
	}
	else
	{
		instr ->prefixes |= INSTR_PREFIX_LOCK;
		res = 1;
	}

	return res;
}

static void pref_CS_clear(struct INSTRUCTION *instr)
{
	instr ->prefixes &= ~INSTR_PREFIX_CS;
}

static void pref_DS_clear(struct INSTRUCTION *instr)
{
	instr ->prefixes &= ~INSTR_PREFIX_DS;
}

static void pref_SS_clear(struct INSTRUCTION *instr)
{
	instr ->prefixes &= ~INSTR_PREFIX_SS;
}

static void pref_ES_clear(struct INSTRUCTION *instr)
{
	instr ->prefixes &= ~INSTR_PREFIX_ES;
}

static void pref_FS_clear(struct INSTRUCTION *instr)
{
	instr ->prefixes &= ~INSTR_PREFIX_FS;
}

static void pref_GS_clear(struct INSTRUCTION *instr)
{
	instr ->prefixes &= ~INSTR_PREFIX_GS;
}

static void pref_REPZ_clear(struct INSTRUCTION *instr)
{
	instr ->prefixes &= ~INSTR_PREFIX_REPZ;
}

static void pref_REPNZ_clear(struct INSTRUCTION *instr)
{
	instr ->prefixes &= ~INSTR_PREFIX_REPNZ;
}

static void pref_OPSIZE_clear(struct INSTRUCTION *instr)
{
	instr ->prefixes &= ~INSTR_PREFIX_OPSIZE;
}

static void pref_ADDRSIZE_clear(struct INSTRUCTION *instr)
{
	instr ->prefixes &= ~INSTR_PREFIX_ADDRSIZE;
}

static void pref_REX_clear(struct INSTRUCTION *instr)
{
	instr ->prefixes &= ~INSTR_PREFIX_REX;
}

uint32_t post_proc_arpl_movsxd(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, uint8_t mode)
{
	uint32_t res;

	res = 0;
	if (mode == DISASSEMBLE_MODE_64)
	{
		struct OPERAND_SIZE opsize;

		instr ->id = ID_MOVSXD;
		strcpy(instr ->mnemonic, "movsxd");
		offset += instr ->opcode_offset + 1;
		if (instr ->flags & INSTR_FLAG_MODRM)
		{
			offset++;
			res++;
		}
		if (instr ->flags & INSTR_FLAG_SIB)
		{
			offset++;
			res++;
		}
		memset(instr ->ops, 0x0, sizeof(instr ->ops));
		memset(instr ->ops + 1, 0x0, sizeof(instr ->ops + 1));
		instr ->ops[0].flags = OPERAND_PRESENT;
		instr ->ops[1].flags = OPERAND_PRESENT;

		ops_dqp(instr, mode, &opsize);
		res += opt_G(origin_offset, offset, instr, instr ->ops, &opsize, mode);
		if (GET_ERR(res))
			return res;
		ops_d(instr, mode, &opsize);
		res += opt_E(origin_offset, offset, instr, instr ->ops + 1, &opsize, mode);
	}

	return res;
}

uint32_t post_proc_nop_pause(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, uint8_t mode)
{
	if (instr ->prefixes & INSTR_PREFIX_REPNZ)
	{
		instr ->id = ID_PAUSE;
		instr ->prefixes &= ~INSTR_PREFIX_REPNZ;
		strcpy(instr ->mnemonic, "pause");
	}

	return 0;
}

uint32_t post_proc_multinop(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, uint8_t mode)
{
	instr ->ops[0].flags &= ~OPERAND_PRESENT;
	instr ->ops[1].flags &= ~OPERAND_PRESENT;
	instr ->ops[2].flags &= ~OPERAND_PRESENT;

	return 0;
}

uint32_t post_proc_cmpxchg8b(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, uint8_t mode)
{
	if (instr ->prefixes & INSTR_PREFIX_REX && instr ->rex & PREFIX_REX_W)
	{
		strcpy(instr ->mnemonic, "cmpxchg16b");
		instr ->ops[0].size = OPERAND_SIZE_128;
	}

	return 0;
}

static void movsx(void *value, unsigned int size1, unsigned int size2)
{
	uint8_t msb;

	if (size1 < size2)
	{
		msb = *((uint8_t *)((uint8_t *)value + size1 - 1));
		if (msb & 0x80)
			memset((uint8_t *)value + size1, 0xFF, size2 - size1);
		else
			memset((uint8_t *)value + size1, 0x0, size2 - size1);
	}
}

static uint8_t bsr(uint32_t src)
{
	uint8_t res;

	for(res = 0; src; src >>= 0x1)
	{
		res++;
		if (src & 0x1)
			break;
	}

	return res;
}

static void create_reg_operand(struct OPERAND *op, uint8_t type, uint8_t code, uint16_t size)
{
	op ->flags |= OPERAND_TYPE_REG;
	op ->value.reg.type = type;
	op ->value.reg.code = code;
	op ->size = (uint8_t)size;
}

static void create_genreg_operand(struct INSTRUCTION *instr, struct OPERAND *op, uint8_t code, uint16_t size, uint8_t rex, uint8_t mode)
{
	if (mode == DISASSEMBLE_MODE_64 && instr ->prefixes & INSTR_PREFIX_REX)
	{
		if (instr ->rex & rex)
			code |= REG_CODE_64;
		else if (code > REG_CODE_BX && size == OPERAND_SIZE_8)
		{
			code |= REG_CODE_64;
			code += 0x4;
		}
	}
	create_reg_operand(op, REG_TYPE_GEN, code, size);
}

static void create_xmmreg_operand(struct INSTRUCTION *instr, struct OPERAND *op, uint8_t code, uint16_t size, uint8_t mode)
{
	if ((mode == DISASSEMBLE_MODE_64) && (instr ->prefixes & INSTR_PREFIX_REX))
	{
		if (instr ->rex & PREFIX_REX_R)
			code |= REG_CODE_64;
	}
	create_reg_operand(op, REG_TYPE_XMM, code, size);
}

static uint8_t get_operand_size_16_32(struct INSTRUCTION *instr, uint8_t mode)
{
	uint8_t res;
	uint16_t prefixes = instr ->prefixes;

	if (mode == DISASSEMBLE_MODE_64)
		mode = DISASSEMBLE_MODE_32;

	if (prefixes & INSTR_PREFIX_OPSIZE)
		mode ^= (DISASSEMBLE_MODE_16 | DISASSEMBLE_MODE_32);

	if (mode == DISASSEMBLE_MODE_16)
		res = OPERAND_SIZE_16;
	else
		res = OPERAND_SIZE_32;

	return res;
}

static uint8_t get_operand_size(struct INSTRUCTION *instr, uint8_t mode)
{
	uint8_t res;

	if (mode == DISASSEMBLE_MODE_64)
	{
		if ((instr ->flags & INSTR_FLAG_D64) ||
			(instr ->flags & INSTR_FLAG_F64) ||
			((instr ->prefixes & INSTR_PREFIX_REX) && (instr ->rex & PREFIX_REX_W)) )
		{
			res = OPERAND_SIZE_64;
		}
		else
		{
			res = get_operand_size_16_32(instr, mode);
		}
	}
	else
	{
		res = get_operand_size_16_32(instr, mode);
	}

	return res;
}

static void get_address_size(struct INSTRUCTION *instr, uint8_t mode)
{
	if (mode == DISASSEMBLE_MODE_64)
	{
		if (instr ->prefixes & INSTR_PREFIX_ADDRSIZE)
			instr ->addrsize = ADDR_SIZE_32;
		else
			instr ->addrsize = ADDR_SIZE_64;
	}
	else
	{
		if (instr ->prefixes & INSTR_PREFIX_ADDRSIZE)
			mode ^= (DISASSEMBLE_MODE_16 | DISASSEMBLE_MODE_32);

		if (mode == DISASSEMBLE_MODE_16)
			instr ->addrsize = ADDR_SIZE_16;
		else
			instr ->addrsize = ADDR_SIZE_32;
	}
}

static void get_seg(struct INSTRUCTION *instr, struct OPERAND *op, uint8_t mode)
{

	if ( !(instr ->prefixes & INSTR_PREFIX_SEG_MASK) )
	{
		if (mode == DISASSEMBLE_MODE_64)
		{
			op ->value.addr.seg = SREG_CODE_CS;
		}
		else
		{
			if ( op ->value.addr.mod == ADDR_MOD_DISP ||
				((op ->value.addr.base != REG_CODE_BP) && (op ->value.addr.base != REG_CODE_SP)) )
			{
				op ->value.addr.seg = SREG_CODE_DS;
			}
			else
			{
				op ->value.addr.seg = SREG_CODE_SS;
			}
		}
	}
	else
	{
		if (mode == DISASSEMBLE_MODE_64)
		{
			switch(instr ->prefixes & INSTR_PREFIX_SEG_MASK)
			{
			case INSTR_PREFIX_FS:
				op ->value.addr.seg = SREG_CODE_FS;
				break;
			case INSTR_PREFIX_GS:
				op ->value.addr.seg = SREG_CODE_GS;
				break;
			default:
				op ->value.addr.seg = SREG_CODE_CS;
				break;
			}
		}
		else
		{
			uint8_t idx;

			idx = bsr(instr ->prefixes & INSTR_PREFIX_SEG_MASK);
			op ->value.addr.seg = sregs[idx - 1];
		}
	}
}

static void get_mod_type_common(struct INSTRUCTION *instr, struct OPERAND *op)
{
	if (((op ->value.addr.base == REG_CODE_BP)  ||
		(op ->value.addr.base == REG_CODE_R13)) &&
		(op ->value.addr.mod == 0x0))
	{
		op ->value.addr.mod = ADDR_MOD_DISP;
	}
	else
	{
		if ((instr ->disp.size == 0) ||	(instr ->disp.value.d64 == 0))
		{
			op ->value.addr.mod = ADDR_MOD_BASE;
		}
		else
		{
			op ->value.addr.mod = ADDR_MOD_BASE | ADDR_MOD_DISP;
		}
	}
}

static void get_mod_type_sib(struct INSTRUCTION *instr, struct OPERAND *op)
{
	if (op ->value.addr.index == REG_CODE_SP)
	{
		get_mod_type_common(instr, op);
	}
	else
	{
		if (op ->value.addr.mod == 0)
		{
			if ( (op ->value.addr.base == REG_CODE_BP) || (op ->value.addr.base == REG_CODE_R13) )
			{
				if (instr ->disp.value.d64 == 0)
					op ->value.addr.mod = ADDR_MOD_IDX;
				else
					op ->value.addr.mod = ADDR_MOD_DISP;
			}
			else
			{
				op ->value.addr.mod = ADDR_MOD_BASE | ADDR_MOD_IDX;
			}
		}
		else
		{
			if (instr ->disp.value.d64 == 0)
				op ->value.addr.mod = ADDR_MOD_BASE | ADDR_MOD_IDX;
			else
				op ->value.addr.mod = ADDR_MOD_BASE | ADDR_MOD_IDX | ADDR_MOD_DISP;
		}
	}
}

static void get_mod_type_modrm(struct INSTRUCTION *instr, struct OPERAND *op, uint8_t mode)
{
	if (mode == DISASSEMBLE_MODE_64)
	{
		if ((op ->value.addr.mod == 0x0) &&
			((op ->value.addr.base == REG_CODE_BP)  ||
			(op ->value.addr.base == REG_CODE_R13)))
		{
			op ->value.addr.base = REG_CODE_IP;
		}
	}
	get_mod_type_common(instr, op);
}

static uint8_t get_disp(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, uint8_t mode)
{
	uint8_t len;

	switch(op ->value.addr.mod)
	{
	case 0x0:
		if (op ->value.addr.base == REG_CODE_BP)
			len = 0x4;
		else
			len = 0x0;
		break;
	case 0x1:
		len = 0x1;
		break;
	case 0x2:
		if (mode == DISASSEMBLE_MODE_16)
			len = 0x2;
		else
			len = 0x4;
	}

	instr ->disp.size = len;
	if (len)
	{
		memcpy(&(instr ->disp.value), offset, len);
		//movsx(&(instr ->disp.value), len, instr ->addrsize);
		movsx(&(instr ->disp.value), len, 0x8);
		instr ->disp.offset = (uint8_t)(offset - origin_offset);
	}

	return len;
}

static uint32_t parse_mem_operand_16(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, uint8_t mode)
{
	uint8_t len;
	unsigned int index;

	op ->value.addr.mod = instr ->modrm >> 0x6;
	len = get_disp(origin_offset, offset, instr, op, mode);
	index = (instr ->modrm >> 0x3 & 0x18) | (instr ->modrm & 0x7);
	op ->value.addr.seg = addrs_16bit[index].seg;
	op ->value.addr.mod = addrs_16bit[index].mod;
	op ->value.addr.base = addrs_16bit[index].base;
	op ->value.addr.index = addrs_16bit[index].index;
	op ->value.addr.scale = addrs_16bit[index].scale;

	return len;
}

static uint32_t parse_mem_operand_32_64(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, uint8_t mode)
{
	uint32_t len = 0;

	if (instr ->flags & INSTR_FLAG_SIB)
	{
		op ->value.addr.mod = instr ->modrm >> 0x6;
		op ->value.addr.base = instr ->sib & 0x7;
		op ->value.addr.index = (instr ->sib >> 3) & 0x7;
		op ->value.addr.scale = 1 << (instr ->sib >> 0x6);

		if (mode == DISASSEMBLE_MODE_64 && instr ->prefixes & INSTR_PREFIX_REX)
		{
			if (instr ->rex & PREFIX_REX_B)
				op ->value.addr.base |= REG_CODE_64;
			if (instr ->rex & PREFIX_REX_X)
				op ->value.addr.index |= REG_CODE_64;
		}
		len = get_disp(origin_offset, offset, instr, op, mode);
		get_mod_type_sib(instr, op);
	}
	else
	{
		op ->value.addr.mod = instr ->modrm >> 0x6;
		op ->value.addr.base = instr ->modrm & 0x7;

		if (mode == DISASSEMBLE_MODE_64 && instr ->prefixes & INSTR_PREFIX_REX)
		{
			if (instr ->rex & PREFIX_REX_B)
				op ->value.addr.base |= REG_CODE_64;
		}
		len = get_disp(origin_offset, offset, instr, op, mode);
		get_mod_type_modrm(instr, op, mode);
	}
	get_seg(instr, op, mode);

	return len;
}

static uint32_t parse_mem_operand(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t len;

	op ->flags |= OPERAND_TYPE_MEM;
	op ->size = opsize ->size;
	if (instr ->addrsize == ADDR_SIZE_16)
	{
		len = parse_mem_operand_16(origin_offset, offset, instr, op, mode);
	}
	else
	{
		len = parse_mem_operand_32_64(origin_offset, offset, instr, op, mode);
	}

	return len;
}

static uint32_t parse_rm_operand(uint8_t *origin_offset, uint8_t *offset, struct INSTRUCTION *instr, struct OPERAND *op, struct OPERAND_SIZE *opsize, uint8_t mode)
{
	uint32_t len = 0;

	if ((instr ->modrm & 0xC0) == 0xC0)
	{
		create_genreg_operand(instr, op, instr ->modrm & 0x7, opsize ->size, PREFIX_REX_B, mode);
	}
	else
	{
		len = parse_mem_operand(origin_offset, offset, instr, op, opsize, mode);
	}

	return len;
}

uint32_t parse_operand(uint8_t *origin_offset, uint8_t *offset, struct INTERNAL_OPERAND *iop, struct INSTRUCTION *instr, struct OPERAND *op, uint8_t mode)
{
	uint32_t res = 0;
	struct OPERAND_SIZE opsize;

	if (iop ->type == TQ_NULL)
		return 0;

	op ->flags |= OPERAND_PRESENT;
	if (iop ->size >= sizeof(ops_handlers) / sizeof(ops_handlers[0]))
		return MAKE_ERR(res, ERR_INTERNAL);//error: internal error.
	else
		ops_handlers[iop ->size](instr, mode, &opsize);

	if (iop ->size >= sizeof(opt_handlers) / sizeof(opt_handlers[0]))
		return MAKE_ERR(res, ERR_INTERNAL);//error: internal error.
	else
		res = opt_handlers[iop ->type](origin_offset, offset, instr, op, &opsize, mode);

	return res;
}

void copy_eflags(struct INSTRUCTION *instr, struct OPCODE *opcode)
{
	instr ->tested_flags = opcode ->tested_flags;
	instr ->modified_flags = opcode ->modified_flags;
	instr ->set_flags = opcode ->set_flags;
	instr ->cleared_flags = opcode ->cleared_flags;
	instr ->undefined_flags = opcode ->undefined_flags;
}

uint8_t parse_modrm(uint8_t *offset, struct INSTRUCTION *instr, struct OPCODE *opcode)
{
	uint8_t len = 0;

	if (opcode ->props & PROP_MODRM)
	{
		len++;
		instr ->flags |= INSTR_FLAG_MODRM;
		instr ->modrm = *offset;
		if (instr ->addrsize != ADDR_SIZE_16)
		{
			if ((instr ->modrm & 0x7) == 0x4 && (instr ->modrm & 0xC0) != 0xC0)
			{
				len++;
				instr ->flags |= INSTR_FLAG_SIB;
				instr ->sib = offset[1];
			}
		}
	}

	return len;
}

void check_seg_sf_prefixes(struct INSTRUCTION *instr, struct PARAMS *params)
{
	unsigned int i;
	struct OPERAND *op;

	op = NULL;
	for (i = 0; i < 3; i++)
	{
		if (instr ->ops[i].flags & OPERAND_TYPE_MEM)
		{
			op = instr ->ops + i;
			break;
		}
	}

	if (instr ->prefixes & INSTR_PREFIX_SEG_MASK)
	{
		if (!op)
		{
			uint8_t idx;

			idx = bsr(instr ->prefixes & INSTR_PREFIX_SEG_MASK);
			idx--;
			instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
			pref_clear_handlers[idx](instr);

			if (params ->sf_prefixes)
				params ->sf_prefixes[params ->sf_prefixes_len] = pref_opcodes[idx];
		}
		else
		{
			if (params ->mode == DISASSEMBLE_MODE_64)
			{
				if ( !((instr ->prefixes & INSTR_PREFIX_FS) || (instr ->prefixes & INSTR_PREFIX_GS)) )
				{
					instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
					pref_clear_handlers[op ->value.addr.seg](instr);
					op ->value.addr.seg = SREG_CODE_CS;

					if (params ->sf_prefixes)
						params ->sf_prefixes[params ->sf_prefixes_len] = pref_opcodes[op ->value.addr.seg];
				}
			}
			else
			{
				if ( (instr ->prefixes & INSTR_PREFIX_SS)   &&
					 (op ->value.addr.mod != ADDR_MOD_DISP) &&
					 ((op ->value.addr.base == REG_CODE_BP) || (op ->value.addr.base == REG_CODE_SP)) )
				{
					instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
					pref_clear_handlers[op ->value.addr.seg](instr);

					if (params ->sf_prefixes)
						params ->sf_prefixes[params ->sf_prefixes_len] = pref_opcodes[op ->value.addr.seg];
				}
				else if (instr ->prefixes & INSTR_PREFIX_DS)
				{
					instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
					pref_clear_handlers[op ->value.addr.seg](instr);

					if (params ->sf_prefixes)
						params ->sf_prefixes[params ->sf_prefixes_len] = pref_opcodes[op ->value.addr.seg];
				}
			}
		}
	}
}

void check_rex_sf_prefix(struct INSTRUCTION *instr, struct PARAMS *params)
{
	// Well, it is possible to make a big one 'if' construction,
	// but I think that this way is better. Anyway, nobody cares.
	if (instr ->prefixes & INSTR_PREFIX_REX)
	{
		if ((instr ->flags & INSTR_FLAG_D64) || (instr ->flags & INSTR_FLAG_F64))
		{
			if (instr ->rex == PREFIX_REX_W)
			{
				instr ->prefixes |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
				instr ->prefixes &= ~INSTR_PREFIX_REX;
				if (params ->sf_prefixes)
				{
					params ->sf_prefixes[params->sf_prefixes_len++] = instr ->rex;
				}
			}
		}
	}
}

void check_sse_sf_prefixes(struct INSTRUCTION *instr, uint8_t sse_pref, uint8_t sf_prefixes[], uint8_t mode)
{
	uint8_t  idx;
	uint16_t prefixes;
	struct OPCODE *ptr;

	prefixes = instr ->prefixes;
	pref_OPSIZE_clear(instr);
	pref_REPZ_clear(instr);
	pref_REPNZ_clear(instr);

	ptr = &(tables[IDX_1BYTE].opcodes[sse_pref]);
	opt_handlers[ptr ->ops[0].type](NULL, NULL, instr, NULL, NULL, mode);

	prefixes ^= instr ->prefixes;
	prefixes &= (INSTR_PREFIX_OPSIZE | INSTR_PREFIX_REP_MASK);
	while (prefixes)
	{
		idx = bsr(prefixes);
		pref_clear_handlers[idx - 0x1](instr);
		instr ->flags |= INSTR_FLAG_SUPERFLUOUS_PREFIX;
		if (sf_prefixes)
		{
			sf_prefixes[strlen(sf_prefixes)] = pref_opcodes[idx - 1];
		}
	}
}

uint32_t parse_prefixes(uint8_t *offset, struct INSTRUCTION *instr, uint8_t *pref_index, struct PARAMS *params)
{
	uint8_t pref_code;
	uint8_t rex_found;
	uint8_t prefixes[MAX_INSTRUCTION_LEN];
	uint32_t res;
	unsigned int prefixes_count;
	unsigned int sf_prefixes_count;
	int i;
	struct OPCODE *ptr;

	res = 0;
	prefixes_count = sf_prefixes_count = rex_found = 0;
	while(1)
	{
		pref_code = *offset;
		if (GET_LEN(res) >= MAX_INSTRUCTION_LEN)
			return MAKE_ERR(res, ERR_TOO_LONG);//error: instruction too long.

		ptr = &(tables[IDX_1BYTE].opcodes[pref_code]);
		if ( !( (ptr ->groups & GRP_PREFIX) ||
			    (params ->mode == DISASSEMBLE_MODE_64 && pref_code >= 0x40 && pref_code <= 0x4F) ) )
		{
			break;
		}
		else
		{
			if (rex_found)
				res = MAKE_ERR(res, ERR_REX_NOOPCD);//error: an opcode should follow after rex.

			if (params ->mode == DISASSEMBLE_MODE_64)
			{
				if ( (pref_code >= 0x40) && (pref_code <= 0x4F) )
				{
					instr ->prefixes |= INSTR_PREFIX_REX;
					instr ->rex = *offset;
					rex_found = 1;
				}
			}

			//Collect all prefixes to parse them later.
			prefixes[prefixes_count++] = pref_code;

			//Used laster for prefix table switch.
			if (ptr ->id == ID_66 || ptr ->id == ID_REPZ || ptr ->id == ID_REPNZ)
				*pref_index = ptr ->props & 0xFF;

			res++;
			offset++;
		}
	}

	params ->sf_prefixes_len = 0;
	for(i = prefixes_count - 1; i >= 0; i--)
	{
		ptr = &(tables[IDX_1BYTE].opcodes[prefixes[i]]);
		if (prefixes[i] >= 0x40 && prefixes[i] <= 0x4F)
			continue;
		else
		{
			if ( !opt_handlers[ptr ->ops[0].type](NULL, NULL, instr, NULL, NULL, params ->mode) )
				if (params ->sf_prefixes)
					params ->sf_prefixes[params ->sf_prefixes_len++] = prefixes[i];
		}
	}

	return res;
}

uint32_t lookup_opcode(uint8_t *offset, uint8_t table, struct OPCODE **opcode_descr)
{
	uint8_t max;
	uint8_t opcode;
	uint32_t res;

	res = 1;
	*opcode_descr = NULL;
	do
	{
		opcode = *offset;
		opcode >>= tables[table].shift;
		opcode &= tables[table].mask;
		opcode -= tables[table].min;
		//It looks strange, but I that table descriptors contain
		// "real" max values.
		max = tables[table].max - tables[table].min;
		if (opcode > max)
		{
			res = MAKE_ERR(res, ERR_BADCODE);
			break;
		}

		if ( !(tables[table].opcodes[opcode].groups & GRP_SWITCH) )
		{
			*opcode_descr = &(tables[table].opcodes[opcode]);
			break;
		}
		else
		{
			if ( !(tables[table].opcodes[opcode].props & PROP_MODRM) )
				res++;
			table = tables[table].opcodes[opcode].props & 0xFF;
		}
		offset++;
	}
	while(1);

	return res;
}

uint32_t parse_opcode(uint8_t *offset, struct OPCODE **opcode_descr, struct INSTRUCTION *instr, struct PARAMS *params)
{
	uint8_t pref_table_index;
	uint32_t res;
	uint32_t tmp;

	pref_table_index = 0xFF;
	res = parse_prefixes(offset, instr, &pref_table_index, params);
	if (GET_ERR(res) == ERR_TOO_LONG)
		return res;

	instr ->opcode_offset = GET_LEN(res);
	offset += GET_LEN(res);

	if (pref_table_index != 0xFF && *offset == 0xF)
	{
		tmp = lookup_opcode(offset, pref_table_index, opcode_descr);
		if (IS_OK(tmp) && (*opcode_descr) ->id != ID_NULL)
		{
			res += tmp;
		}
		else
		{
			res += lookup_opcode(offset, IDX_1BYTE, opcode_descr);
		}
	}
	else
	{
		res += lookup_opcode(offset, IDX_1BYTE, opcode_descr);
	}

	if (IS_OK(res) && (*opcode_descr) ->id == ID_NULL)
	{
		res = MAKE_ERR(res, ERR_BADCODE);//error: invalid opcode.
	}
	if (GET_LEN(res) > MAX_INSTRUCTION_LEN)
	{
		res = MAKE_ERR(res, ERR_TOO_LONG);
	}

	return res;
}

unsigned int disassemble(uint8_t *offset, struct INSTRUCTION *instr, struct PARAMS *params)
{
	uint8_t err;
	uint8_t len;
	uint32_t res;
	struct OPCODE *opcode;

	//Clear everything.
	memset(instr, 0x0, sizeof(*instr));
	len = err = 0;
	res = 0;

	//Lookup opcode.
	res = parse_opcode(offset, &opcode, instr, params);
	err = GET_ERR(res);
	if (err == ERR_TOO_LONG || err == ERR_BADCODE)
		return res;//error: error set in parse_opcode.
	len += GET_LEN(res);
	get_address_size(instr, params ->mode);
	if (opcode ->props & PROP_D64)
		instr ->flags |= INSTR_FLAG_D64;
	if (opcode ->props & PROP_F64)
		instr ->flags |= INSTR_FLAG_F64;

	//Parse MODRM byte.
	len += parse_modrm(offset + len, instr, opcode);
	if (len > MAX_INSTRUCTION_LEN)
		return MAKE_ERR(res, ERR_TOO_LONG);//error: instruction too long.

	//Copy flags, id, groups.
	copy_eflags(instr, opcode);
	instr ->id = opcode ->id;
	instr ->groups = opcode ->groups;

	strcpy(instr ->mnemonic, opcode ->mnemonic);
	//Deal with operands.
	res = parse_operand(offset, offset + len, opcode ->ops, instr, instr ->ops, params ->mode);
	len += GET_LEN(res);
	if (GET_ERR(res) == ERR_INTERNAL)
		return res;
	if (len > MAX_INSTRUCTION_LEN)
		return MAKE_ERR(res, ERR_TOO_LONG);//error: instruction too long.
	if (!err)
		err = GET_ERR(res);

	res = parse_operand(offset, offset + len, opcode ->ops + 1, instr, instr ->ops + 1, params ->mode);
	len += GET_LEN(res);
	if (GET_ERR(res) == ERR_INTERNAL)
		return res;
	if (len > MAX_INSTRUCTION_LEN)
		return MAKE_ERR(res, ERR_TOO_LONG);//error: instruction too long.
	if (!err)
		err = GET_ERR(res);

	res = parse_operand(offset, offset + len, opcode ->ops + 2, instr, instr ->ops + 2, params ->mode);
	len += GET_LEN(res);
	if (GET_ERR(res) == ERR_INTERNAL)
		return res;
	if (len > MAX_INSTRUCTION_LEN)
		return MAKE_ERR(res, ERR_TOO_LONG);//error: instruction too long.
	if (!err)
		err = GET_ERR(res);

	//Check if REX is superfluous.
	check_rex_sf_prefix(instr, params);
	//Check if segment prefix is superfluous.
	check_seg_sf_prefixes(instr, params);
	//Check if opsize or addrsize are superfluous. 
	//check_size_sf_prefixes(instr, sf_prefixes, )

	//Let's do postprocessing.
	if (opcode ->props & PROP_POST_PROC)
	{
		res = postprocs[(opcode ->props >> 0x8) & 0xF](offset, offset, instr, params ->mode);
		if (res)
		{
			len = instr ->opcode_offset + 1;
			len += GET_LEN(res);
			if (GET_ERR(res) == ERR_INTERNAL)
				return res;
			if (len > MAX_INSTRUCTION_LEN)
				return MAKE_ERR(res, ERR_TOO_LONG);
			if (!err)
				err = GET_ERR(res);
		}
	}

	//And post checks. Only if err is not set.
	if (!err)
	{
		if (!(params ->arch & opcode ->arch))
			err = ERR_ANOT_ARCH;//error: another architecture.
		else if (instr ->prefixes & INSTR_PREFIX_LOCK && (! (opcode ->props & PROP_LOCK)) )
			err = ERR_NON_LOCKABLE;//error: prefix lock non-lockable instruction.
		else if	((opcode ->props & PROP_I64) && (params ->mode == DISASSEMBLE_MODE_64))
			err = ERR_16_32_ONLY;//error: instruction is 16/32bit mode only.
		else if (opcode ->props & PROP_O64 && params ->mode != DISASSEMBLE_MODE_64)
			err = ERR_64_ONLY;//error: instruction is 64bit mode only.
	}

	return MAKE_ERR(len, err);
}