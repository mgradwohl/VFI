#ifndef TCRITICALSECTION_H
#pragma once

#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
	#endif
#endif

#include <windows.h>

#ifndef TRACE
	#ifdef TEST
		#define TRACE(s)	{	\
							ErrorMessageBox(NULL, GetLastError(), L"VFI Trace", s);	\
						}	\
	#else
		#define TRACE(s)
	#endif
#endif

class TCriticalSection  
{
public:
	TCriticalSection()
	{
		InitializeCriticalSection(&m_cs);
	}

	virtual ~TCriticalSection()
	{
		DeleteCriticalSection(&m_cs);
	}

	void Enter()
	{
		EnterCriticalSection(&m_cs);
	}

	void Leave()
	{
		LeaveCriticalSection(&m_cs);
	}
private:
	CRITICAL_SECTION m_cs;
};

#ifndef//TCRITICALSECTION_H
#define TCRITICALSECTION_H

