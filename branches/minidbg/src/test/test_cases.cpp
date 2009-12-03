#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/ref.hpp>
#include <string>
#include <iostream>

#include "tracer.h"

void trace_slot(dbg_msg msg)
{
	std::cout << "trace slot\n";
}

void breakpoint_slot(dbg_msg msg)
{
	std::cout << "breakpoint slot\n";
}

void terminated_slot(dbg_msg msg)
{
	BOOST_CHECK(msg.process_id);
	BOOST_CHECK(msg.thread_id);
	BOOST_CHECK(msg.terminated.proc_id);
	std::cout << "terminated slot\n";
}

void start_thread_slot(dbg_msg msg)
{
	BOOST_CHECK(msg.process_id);
	BOOST_CHECK(msg.thread_id);
	BOOST_CHECK(msg.thread_start.thread_id);
	std::cout << "start thread slot\n";
}

void exit_thread_slot(dbg_msg msg)
{
	BOOST_CHECK(msg.process_id);
	BOOST_CHECK(msg.thread_id);
	BOOST_CHECK(msg.thread_exit.thread_id);
	BOOST_CHECK(msg.thread_exit.proc_id);
	std::cout << "exit thread slot\n";
}

void exception_slot(dbg_msg msg)
{
	BOOST_CHECK(msg.process_id);
	BOOST_CHECK(msg.thread_id);
	BOOST_CHECK(msg.exception.except_record.ExceptionCode);
	std::cout << "exception slot\n";
}

void dll_load_slot(dbg_msg msg)
{
	BOOST_CHECK(msg.process_id);
	BOOST_CHECK(msg.thread_id);
	BOOST_CHECK(msg.dll_load.dll_name);
	BOOST_CHECK(msg.dll_load.dll_image_base);
	std::wcout << "dll load " << msg.dll_load.dll_name << "\n";
	trc::enable_single_step(msg.process_id, msg.thread_id);
}

void test_events_handling()
{
	std::string image_name("c:\\windows\\notepad.exe");
	trc::tracer tracer_obj;
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

boost::unit_test_framework::test_suite* init_unit_test_suite(int, char*[])
{
	boost::unit_test_framework::test_suite* test =
		BOOST_TEST_SUITE("opendbg: tracer test suites");

	test->add(BOOST_TEST_CASE(test_events_handling));

	return test;
}
