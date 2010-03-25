#pragma once

//#ifdef _WIN32
//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
//#endif

#include <cstdio>
#include <tchar.h>
#include <windows.h>
#include <Psapi.h>

#include <vector>
#include <cstdio>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <fstream>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>

#include "udis86.h"

#include "defines.h"
typedef unsigned long u_long;