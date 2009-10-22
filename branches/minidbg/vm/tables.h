struct TABLE_DESCRIPTOR
{
	uint8_t min;
	uint8_t max;
	uint8_t mask;
	uint8_t shift;
	struct OPCODE *opcodes;
};

struct INTERNAL_OPERAND
{
	uint8_t type;
	uint8_t size;
};

#pragma pack(push, 0x4)
struct OPCODE
{
	uint64_t groups;
	unichar_t *mnemonic;
	uint16_t id;
	struct INTERNAL_OPERAND ops[3];
	uint16_t props;
	uint8_t tested_flags;
	uint8_t modified_flags;
	uint8_t set_flags;
	uint8_t cleared_flags;
	uint8_t undefined_flags;
	uint8_t arch;
};
#pragma pack(pop)

struct OPERAND_SIZE
{
	uint16_t size_in_stream;
	uint16_t size;
	uint8_t sign;
};

#define PROP_LOCK      0x1
#define PROP_I64       0x2
#define PROP_D64       0x4
#define PROP_F64       0x8
#define PROP_O64       0x10
#define PROP_IOPL      0x20
#define PROP_RING0     0x40
#define PROP_SERIAL    0x80
#define PROP_OPSIZE    0x2000
#define PROP_POST_PROC 0x4000
#define PROP_MODRM     0x8000

#define TQ_NULL 0xFF
#define TQ_1    0x0
#define TQ_3    0x1
#define TQ_A    0x2
#define TQ_C    0x3
#define TQ_D    0x4
#define TQ_E    0x5
#define TQ_G    0x6
#define TQ_H    0x7
#define TQ_I    0x8
#define TQ_J    0x9
#define TQ_M    0xA
#define TQ_N    0xB
#define TQ_O    0xC
#define TQ_P    0xD
#define TQ_Q    0xE
#define TQ_R    0xF
#define TQ_S    0x10
#define TQ_T    0x11
#define TQ_U    0x12
#define TQ_V    0x13
#define TQ_W    0x14
#define TQ_X    0x15
#define TQ_Y    0x16
#define TQ_Z    0x17
#define TQ_rAX  0x18
#define TQ_rCX  0x19
#define TQ_rDX  0x1A
#define TQ_rBX  0x1B
#define TQ_rSP  0x1C
#define TQ_rBP  0x1D
#define TQ_rSI  0x1E
#define TQ_rDI  0x1F
#define TQ_fES  0x20
#define TQ_fEST 0x21
#define TQ_fST0 0x22
#define TQ_CS   0x23
#define TQ_DS   0x24
#define TQ_ES   0x25
#define TQ_SS   0x26
#define TQ_FS   0x27
#define TQ_GS   0x28
#define TQ_PREF_CS 0x29
#define TQ_PREF_DS 0x2A
#define TQ_PREF_ES 0x2B
#define TQ_PREF_SS 0x2C
#define TQ_PREF_FS 0x2D
#define TQ_PREF_GS 0x2E
#define TQ_PREF_66 0x2F
#define TQ_PREF_67 0x30
#define TQ_PREF_F2 0x31
#define TQ_PREF_F3 0x32
#define TQ_PREF_F0 0x33

#define SQ_NULL 0xFF
#define SQ_a    0x0
#define SQ_b    0x1
#define SQ_bcd  0x2
#define SQ_bdqp 0x3
#define SQ_bs   0x4
#define SQ_bss  0x5
#define SQ_d    0x6
#define SQ_ddq  0x7
#define SQ_di   0x8
#define SQ_dq   0x9
#define SQ_dqp  0xA
#define SQ_dr   0xB
#define SQ_ds   0xC
#define SQ_e    0xD
#define SQ_er   0xE
#define SQ_p    0xF
#define SQ_pd   0x10
#define SQ_pi   0x11
#define SQ_ps   0x12
#define SQ_psq  0x13
#define SQ_ptp  0x14
#define SQ_q    0x15
#define SQ_qdq  0x16
#define SQ_qi   0x17
#define SQ_sd   0x18
#define SQ_sr   0x19
#define SQ_ss   0x1A
#define SQ_st   0x1B
#define SQ_stx  0x1C
#define SQ_v    0x1D
#define SQ_vds  0x1E
#define SQ_vqp  0x1F
#define SQ_vs   0x20
#define SQ_w    0x21
#define SQ_wdq  0x22
#define SQ_wdqp 0x23
#define SQ_wi   0x24
#define SQ_wv   0x25
#define SQ_wvqp 0x26

#define IDX_1BYTE   0x0
#define IDX_80      0x1
#define IDX_81      0x2
#define IDX_82      0x3
#define IDX_83      0x4
#define IDX_8F      0x5
#define IDX_C0      0x6
#define IDX_C1      0x7
#define IDX_C6      0x8
#define IDX_C7      0x9
#define IDX_D0      0xA
#define IDX_D1      0xB
#define IDX_D2      0xC
#define IDX_D3      0xD
#define IDX_F6      0xE
#define IDX_F7      0xF
#define IDX_FE      0x10
#define IDX_FF      0x11
#define IDX_D8      0x12
#define IDX_D8_C0   0x13
#define IDX_D9      0x14
#define IDX_D9_C0   0x15
#define IDX_DA      0x16
#define IDX_DA_C0   0x17
#define IDX_DB      0x18
#define IDX_DB_C0   0x19
#define IDX_DC      0x1A
#define IDX_DC_C0   0x1B
#define IDX_DD      0x1C
#define IDX_DD_C0   0x1D
#define IDX_DE      0x1E
#define IDX_DE_C0   0x1F
#define IDX_DF      0x20
#define IDX_DF_C0   0x21

#define IDX_0F      0x22
#define IDX_0F_00   0x23
#define IDX_0F_01   0x24
#define IDX_0F_12   0x25
#define IDX_0F_16   0x26
#define IDX_0F_18   0x27
#define IDX_0F_71   0x28
#define IDX_0F_72   0x29
#define IDX_0F_73   0x2A
#define IDX_0F_AE   0x2B
#define IDX_0F_BA   0x2C
#define IDX_0F_C7   0x2D
#define IDX_JMP_66_0F   0x2E
#define IDX_66_0F71 0x2F
#define IDX_66_0F72 0x30
#define IDX_66_0F73 0x31
#define IDX_66_0FC7 0x32
#define IDX_JMP_F2_0F   0x33
#define IDX_JMP_F3_0F   0x34
#define IDX_F3_0FC7 0x35

#define IDX_0F_38   0x36
#define IDX_66_0F38 0x37
#define IDX_F2_0F38 0x38
#define IDX_0F_3A   0x39
#define IDX_66_0F3A 0x3A

#define IDX_66_0F 0x3B
#define IDX_F2_0F 0x3C
#define IDX_F3_0F 0x3D

#define POST_PROC_ARPL_MOVSXD 0x00
#define POST_PROC_NOP_PAUSE   0x100
#define POST_PROC_MULTINOP    0x200
#define POST_PROC_CMPXCHG8B   0x300