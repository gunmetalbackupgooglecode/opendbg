#include "Thread.h"

namespace sys{

Thread::Thread()
{
}

Thread::Thread(u3264 tid)
{
	m_tid = tid;
}

Thread::~Thread(void)
{
}

bool Thread::haveEmptyDebugRegister() const
{
	return false;
}

u3264 Thread::getTid()
{
	return m_tid;
}

}