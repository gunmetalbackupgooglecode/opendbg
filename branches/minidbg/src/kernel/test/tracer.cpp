#include "tracer.h"

namespace trc
{
uintptr_t CALLBACK get_symbols_callback(
	int sym_type,
	char * sym_name,
	char * sym_subname,
	pdb::pdb_parser& pdb
	)
{
	if (sym_type == SYM_TIMESTAMP)
	{
		return 0x45E53F9C;
	}

	if (sym_type == SYM_NTAPI_NUM)
	{
		if (strcmp(sym_name, "ZwTerminateProcess") == 0) {
			return 0x101;
		}

		if (strcmp(sym_name, "ZwCreateThread") == 0) {
			return 0x035;
		}

		if (strcmp(sym_name, "ZwTerminateThread") == 0) {
			return 0x102;
		}
	}

	wchar_t sym_name_w[255];
	MultiByteToWideChar(
		CP_ACP, 0, sym_name, strlen(sym_name)+1,
		sym_name_w, sizeof(sym_name_w)/sizeof(sym_name_w[0])
	);

	wchar_t sym_subname_w[255];
	if (sym_subname && strlen(sym_subname)) {
		MultiByteToWideChar(
			CP_ACP, 0, sym_subname, strlen(sym_subname)+1,
			sym_subname_w, sizeof(sym_subname_w)/sizeof(sym_subname_w[0])
		);
	}

	if (sym_type == SYM_OFFSET) {
		return pdb.get_symbol(sym_name_w).get_rva();
	}

	if (sym_type == SYM_STRUCT_OFFSET)
		return pdb.get_type(sym_name_w).get_member(sym_subname_w).get_offset();

	return 0;
}

void tracer::open_process(const std::string& filename)
{
	u8 sf_prefixes[MAX_INSTRUCTION_LEN];

	if ( (m_pid = dbg_create_process(filename.c_str(), CREATE_NEW_CONSOLE | DEBUG_ONLY_THIS_PROCESS)) == NULL)
		throw tracer_error("process not started");

	if (dbg_attach_debugger(m_pid) == 0)
		throw tracer_error("debugger not attached");

	params.arch = ARCH_COMMON;
	params.sf_prefixes = sf_prefixes;
	params.mode = DISASSEMBLE_MODE_32;
}

bool tracer::enable_single_step(HANDLE process_id, HANDLE thread_id)
{
	u_long readed, cmdlength;
	u8 buf[MAX_INSTRUCTION_LEN];
	CONTEXT context;
	if (dbg_get_context(thread_id, &context))
	{
		// ��������� ����������� ������ �������
		if (!dbg_read_memory(process_id, (char*)context.Eip, &buf, sizeof(buf), &readed))
			return false;
		cmdlength = disassemble(buf, this->instr, &this->params);
		if (analyser::is_instruction_untraceable(*this->instr)) {
			breakpoint bp((u32)process_id, (u32)thread_id, (u3264)(context.Eip + cmdlength));// ��������� int 3 ����
		}
		// �������� ��� ��� quick-����

		context.EFlags |= TF_BIT;
		dbg_set_context(thread_id, &context);
		return true;
	}

	return false;
}

bool tracer::disable_single_step(HANDLE thread_id)
{
	CONTEXT context;
	if (dbg_get_context(thread_id, &context))
	{
		context.EFlags &= ~TF_BIT;
		dbg_set_context(thread_id, &context);
		return true;
	}

	return false;
}
}