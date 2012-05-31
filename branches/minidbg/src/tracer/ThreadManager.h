#pragma once
#include "Thread.h"
#include <list>

namespace sys{
class ThreadManager
{
public:
	ThreadManager(void);
	~ThreadManager(void);
	int threadNum();
	const std::list<THREAD_PTR>& getThreads() const;

protected:
	std::list<THREAD_PTR> threads;
};
}