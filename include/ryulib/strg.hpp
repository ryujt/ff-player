#ifndef _RYULIB_STRG_H_
#define	_RYULIB_STRG_H_

#include "windows.h"

static char *WideCharToChar(wchar_t *src)
{
	int len = WideCharToMultiByte(CP_ACP, 0, src, -1, NULL, 0, NULL, NULL);
	char *result = new char[len];

	WideCharToMultiByte(CP_ACP, 0, src, -1, result, len, 0, 0);

	return result;
}

#endif // _RYULIB_STRG_H_ 