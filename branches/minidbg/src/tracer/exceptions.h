#pragma once

namespace debugger{
	struct DebuggerException: virtual boost::exception, virtual std::exception{};
	struct NoEmptyDebugResgisterException: virtual DebuggerException{};
	struct CantReadMemoryException: virtual DebuggerException{};
	struct CantWriteMemoryException: virtual DebuggerException{};
}