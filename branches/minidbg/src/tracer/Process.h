#pragma once
#include <boost/shared_ptr.hpp>
#include "ThreadManager.h"

namespace sys{
class Process
{
public:
	Process(void);
	Process(u3264 pid);
	~Process(void);

	ThreadManager& getThreadManager();
	u3264 getPid();

private:
	u3264 m_pid;

protected:
	ThreadManager threadManager;
};
typedef boost::shared_ptr<Process> PROCESS_PTR;

}