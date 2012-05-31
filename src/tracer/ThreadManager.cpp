#include "ThreadManager.h"
namespace sys{

ThreadManager::ThreadManager(void)
{
}

ThreadManager::~ThreadManager(void)
{
}

int ThreadManager::threadNum()
{
	return 5;
}

const std::list<THREAD_PTR>& ThreadManager::getThreads() const
{
	return threads;
}
}
