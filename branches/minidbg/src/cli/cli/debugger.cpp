#include "stdafx.h"
#include "debugger.h"
#include "dbgapi.h"

using namespace std;

debugger::debugger()
{
        m_msg = new dbg_msg;
        // TODO: remove hardcoded reference
        // TODO: debugger_exception class
        if (dbg_initialize_api(0x1234, L"c:\\ntoskrnl.pdb", (dbg_sym_get)get_symbols_callback) != 1)
                throw exception("dbgapi initialization error");
}

u_long debugger::get_version()
{
        return dbg_drv_version();
}

void debugger::debug_process(const string& imageName)
{
        if ((m_pid = dbg_create_process(imageName.c_str(), CREATE_NEW_CONSOLE | DEBUG_ONLY_THIS_PROCESS)) == NULL)
                throw exception("process not started");
//      printf("process started with pid %x\n", pid);

        if (dbg_attach_debugger(m_pid) == 0)
                throw exception("debugger not attached");
//      printf("debugger attached\n");
}

void debugger::test()
{
        m_filter.event_mask  = DBG_EXCEPTION | DBG_TERMINATED | DBG_START_THREAD | DBG_EXIT_THREAD | DBG_LOAD_DLL;
        m_filter.filtr_count = 0;

        if (dbg_set_filter(m_pid, &m_filter) == 0)
                throw exception("dbg_set_filter error");

        printf("debug events filter set up\n");

        do
        {
                u32 continue_status = DBG_CONTINUE;
                if (dbg_get_msg_event(m_pid, m_msg) == 0)
                {
                        printf("get debug message error\n");
                        break;
                }

                if (m_msg->event_code == DBG_TERMINATED)
                {
                        printf("DBG_TERMINATED %x by %x\n",
                                m_msg->terminated.proc_id,
                                m_msg->process_id
                                );

                        continue_status = DBG_CONTINUE;
                        //dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
                }

                if (m_msg->event_code == DBG_START_THREAD)
                {
                        printf("DBG_START_THREAD %x by %x, teb: %x\n",
                                m_msg->thread_start.thread_id,
                                m_msg->process_id,
                                m_msg->thread_start.teb_addr
                                );

                        continue_status = DBG_CONTINUE;
                        //dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
                }

                if (m_msg->event_code == DBG_EXIT_THREAD)
                {
                        printf("DBG_EXIT_THREAD %x in %x by %x\n",
                                m_msg->thread_exit.thread_id,
                                m_msg->thread_exit.proc_id,
                                m_msg->process_id
                                );

                        continue_status = DBG_CONTINUE;
                        //dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
                }

                if (m_msg->event_code == DBG_EXCEPTION)
                {
                        printf("DBG_EXCEPTION %0.8x in %x:%x\n",
                                m_msg->exception.except_record.ExceptionCode,
                                m_msg->thread_id,
                                m_msg->process_id
                                );

                        switch (m_msg->exception.except_record.ExceptionCode)
                        {
                        case EXCEPTION_BREAKPOINT :
                                {
                                        if ( m_msg->exception.first_chance )
                                                continue_status = DBG_CONTINUE ;
                                        else
                                                continue_status = DBG_EXCEPTION_NOT_HANDLED ;
                                }
                                break ;

                        default:
                                continue_status  = DBG_CONTINUE ;
                                break ;
                        }
                        //dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
                }

                if (m_msg->event_code == DBG_LOAD_DLL)
                {
                        printf("DBG_LOAD_DLL %ws adr 0x%p sz 0x%x in %x:%x\n",
                                m_msg->dll_load.dll_name,
                                m_msg->dll_load.dll_image_base,
                                m_msg->dll_load.dll_image_size,
                                m_msg->thread_id,
                                m_msg->process_id
                                );

                        continue_status = DBG_CONTINUE;
                        //dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
                }

                if (!ContinueDebugEvent((u32)m_msg->process_id, (u32)m_msg->thread_id, continue_status))
                        break;
        } while (1);
}

// modified version for xp sp3
uintptr_t
CALLBACK debugger::get_symbols_callback(int sym_type, char * sym_name, char * sym_subname, pdb::pdb_parser& pdb)
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

debugger::~debugger(void)
{
        delete m_msg;
}