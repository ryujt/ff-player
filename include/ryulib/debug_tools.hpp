#ifndef RYULIB_DEBUG_TOOLS_HPP
#define RYULIB_DEBUG_TOOLS_HPP

#include <Windows.h>

using namespace std;

class DebugOutput {
public:
	static void trace(const char* format, ...) {
		char buffer[4096];
		va_list vaList;
		va_start(vaList, format);
		_vsnprintf_s(buffer, 4096, format, vaList);
		va_end(vaList);
		OutputDebugStringA(buffer);
	}
};

#endif  // RYULIB_DEBUG_TOOLS_HPP
