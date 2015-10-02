// Visual File Information
// Copyright (c) Microsoft Corporation
// All rights reserved. 
// 
// MIT License
// 
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the ""Software""), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom 
// the Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED *AS IS*, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
// IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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

