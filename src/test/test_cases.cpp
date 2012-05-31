#include "precompiled.h"

#include "breakpoint.h"
#include "tracer.h"

void trace_slot(dbg_msg msg)
{
	//std::cout << "trace slot\n";
}

void breakpoint_slot(dbg_msg msg)
{
	std::cout << "breakpoint address: " << msg.exception.except_record.ExceptionAddress << "\n";
	//std::cout << "breakpoint slot\n";
}

void terminated_slot(dbg_msg msg)
{
	BOOST_CHECK(msg.process_id);
	BOOST_CHECK(msg.thread_id);
	BOOST_CHECK(msg.terminated.proc_id);
	//std::cout << "terminated slot\n";
}

void start_thread_slot(dbg_msg msg)
{
	BOOST_CHECK(msg.process_id);
	BOOST_CHECK(msg.thread_id);
	BOOST_CHECK(msg.thread_start.thread_id);
	//std::cout << "start thread slot\n";
}

void exit_thread_slot(dbg_msg msg)
{
	BOOST_CHECK(msg.process_id);
	BOOST_CHECK(msg.thread_id);
	BOOST_CHECK(msg.thread_exit.thread_id);
	BOOST_CHECK(msg.thread_exit.proc_id);
	//std::cout << "exit thread slot\n";
}

void exception_slot(dbg_msg msg)
{
	BOOST_CHECK(msg.process_id);
	BOOST_CHECK(msg.thread_id);
	BOOST_CHECK(msg.exception.except_record.ExceptionCode);
	//std::cout << "exception slot\n";
}

void dll_load_slot(dbg_msg msg)
{
	BOOST_CHECK(msg.process_id);
	BOOST_CHECK(msg.thread_id);
	BOOST_CHECK(msg.dll_load.dll_name);
	BOOST_CHECK(msg.dll_load.dll_image_base);
	//std::wcout << "dll load " << msg.dll_load.dll_name << "\n";
	//trc::enable_single_step(msg.process_id, msg.thread_id);
}

void test_dbg_attach_debugger()
{
	HANDLE pid;
	BOOST_CHECK(pid = dbg_create_process("c:\\windows\\notepad.exe", CREATE_NEW_CONSOLE | DEBUG_ONLY_THIS_PROCESS));

	ATTACH_INFO attach_info;
	BOOST_CHECK(dbg_attach_debugger(pid, &attach_info));
	
	PVOID ep_addr = reinterpret_cast<PVOID>(
	reinterpret_cast<u3264>(attach_info.image_ep) +
	reinterpret_cast<u3264>(attach_info.image_base)
	);
}

void test_events_handling()
{
	std::string image_name("c:\\windows\\notepad.exe");
	trc::tracer tracer_obj(trc::instruction_set_X86);
	tracer_obj.set_image_name(image_name);
	BOOST_CHECK_EQUAL( tracer_obj.get_image_name(), "c:\\windows\\notepad.exe" );

	tracer_obj.add_trace_slot(trace_slot);
	tracer_obj.add_breakpoint_slot(breakpoint_slot);
	tracer_obj.add_terminated_slot(terminated_slot);
	tracer_obj.add_start_thread_slot(start_thread_slot);
	tracer_obj.add_exit_thread_slot(exit_thread_slot);
	tracer_obj.add_exception_slot(exception_slot);
	tracer_obj.add_dll_load_slot(dll_load_slot);

	boost::thread dbg_thread(boost::ref(tracer_obj));
	dbg_thread.join();
}

void test_step_trace_functions()
{
	trc::tracer tracer_obj(trc::instruction_set_X86);

	u8 loop[] = { 0xE2, 0xE9 };
	BOOST_CHECK(tracer_obj.is_loop(loop));
	u8 loope[] = { 0xE1, 0xE9 };
	BOOST_CHECK(tracer_obj.is_loop(loope));
	u8 loopnz[] = { 0xE0, 0xE9 };
	BOOST_CHECK(tracer_obj.is_loop(loopnz));

	u8 call1[] = { 0xFF, 0x10 };
	BOOST_CHECK(tracer_obj.is_call(call1));
	u8 call2[] = { 0xE8 };
	BOOST_CHECK(tracer_obj.is_call(call2));
	u8 callf[] = { 0x9A, 0xE9 };
	BOOST_CHECK(tracer_obj.is_call(callf));

	u8 rep1[] = { 0xF2, 0x6E };
	BOOST_CHECK(tracer_obj.is_rep(rep1));
	u8 rep2[] = { 0xF3, 0x6E };
	BOOST_CHECK(tracer_obj.is_rep(rep2));

	u8 popss[] = { 0x17, 0x11 };
	BOOST_CHECK(tracer_obj.is_untraceable_opcode(popss));
	u8 popfd[] = { 0x9D, 0x22 };
	BOOST_CHECK(tracer_obj.is_untraceable_opcode(popfd));
	u8 pushfd[] = { 0x9C, 0x33 };
	BOOST_CHECK(tracer_obj.is_untraceable_opcode(pushfd));
}

void test_disasm()
{
	static uint8_t raw[] = { 0x49, 0x49, 0x2A, 0x00, 0xAA, 0x71, 0x00, 0x00,
		0x3B, 0x5B, 0x33, 0xE6, 0x48, 0x32, 0x18, 0x30, 0x90, 0x90, 0x90, 0xC3, 0xC3, 0xC3 };

	ud_t ud_obj;
	ud_init(&ud_obj);
	ud_set_mode(&ud_obj, 32);
	ud_set_input_buffer(&ud_obj, raw, sizeof(raw));
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);

	size_t len = 0;
	while (ud_disassemble(&ud_obj)) {
		len += ud_insn_len(&ud_obj);
		BOOST_CHECK(ud_insn_asm(&ud_obj));
	}

	BOOST_CHECK(len == 22);
	BOOST_CHECK(22 == trc::get_size_till_ret(raw));
}

boost::unit_test_framework::test_suite* init_unit_test_suite(int, char*[])
{
	boost::unit_test_framework::test_suite* test =
		BOOST_TEST_SUITE("opendbg: tracer test suites");

	test->add(BOOST_TEST_CASE(test_dbg_attach_debugger));
	test->add(BOOST_TEST_CASE(test_step_trace_functions));
	test->add(BOOST_TEST_CASE(test_disasm));
	test->add(BOOST_TEST_CASE(test_events_handling));

	return test;
}
