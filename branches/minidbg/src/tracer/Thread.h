#pragma once
#include "types.h"
#include <boost/shared_ptr.hpp>

namespace sys{

class Thread
{
public:
	Thread(void);
	Thread(u3264 tid);
	~Thread(void);
	bool haveEmptyDebugRegister() const;
	u3264 getTid();

private:
	u3264 m_tid;

};

typedef boost::shared_ptr<Thread> THREAD_PTR;
}