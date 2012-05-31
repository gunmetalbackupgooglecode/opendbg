#include "Process.h"
namespace sys{

Process::Process()
{
}

Process::Process(u3264 pid)
{
	m_pid = pid;
}

Process::~Process(void)
{
}

ThreadManager& Process::getThreadManager()
{
	return threadManager;
}

u3264 Process::getPid()
{
	return m_pid;
}

}