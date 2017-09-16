#pragma once
#include <windows.h>
#ifdef _DEBUG
	bool _trace(wchar_t *format, ...);
	#ifndef TRACE
		#define TRACE _trace
	#else
		#define TRACE __noop
	#endif
#endif