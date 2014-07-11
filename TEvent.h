#ifndef TEVENT_H
#pragma once

#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
	#endif
#endif

#include <windows.h>
#include "tguid.h"

#ifdef _DEBUG
	#define MYTRACE(x) OutputDebugString(x)
#else
	#define MYTRACE(x)
#endif

class TEvent  
{
public:
	TEvent()
	{
		m_hEvent = NULL;
	}

	virtual ~TEvent()
	{
		if (NULL != m_hEvent)
		{
			MYTRACE((LPCWSTR) _T("TEvent::~TEvent warning: Closing.\r\n"));
			Close();
		}
	}

	operator HANDLE() const
	{
		return m_hEvent;
	}

	bool Create(LPCWSTR pszName)
	{
		if (NULL == pszName || _T('\0') == *pszName)
		{
			return false;
		}

		if (NULL != m_hEvent)
		{
			MYTRACE((LPCWSTR) _T("TEvent::Create warning: Event handle already used.\r\n"));
			return false;
		}
		
		// no security
		// manual reset
		// initially off
		m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, pszName);

		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			MYTRACE((LPCWSTR) _T("TEvent::Create warning: Event already exists.\r\n"));
		 	return false;
		}

		MYTRACE((LPCWSTR) _T("TEvent::Create: Event created successfully.\r\n"));
		return (NULL != m_hEvent);
	}

	bool Create()
	{
		TGuid guid;
		if (!guid.Create())
			return false;

		return Create(guid.GetString());
	}

	bool Close()
	{
		if (! ::CloseHandle (m_hEvent))
		{
			MYTRACE((LPCWSTR) _T("TEvent::Close warning: CloseHandle failed.\r\n"));
			return false;
		}

		m_hEvent = NULL;
		return true;
	}

	bool Signaled()
	{
		if (NULL == m_hEvent)
		{
			MYTRACE((LPCWSTR) _T("TEvent::Signaled warning: NULL Event.\r\n"));
			return false;
		}

		return (WAIT_OBJECT_0 == ::WaitForSingleObject(m_hEvent, 0));
	}

	bool Signal()
	{
		if (NULL == m_hEvent)
		{
			MYTRACE((LPCWSTR) _T("TEvent::Signal warning: NULL Event.\r\n"));
			return false;
		}

		return (TRUE == ::SetEvent(m_hEvent));
	}

	bool Reset()
	{
		if (NULL == m_hEvent)
		{
			MYTRACE((LPCWSTR) _T("TEvent::Reset warning: NULL Event.\r\n"));
			return false;
		}

		return (TRUE == ::ResetEvent(m_hEvent));
	}

	bool Signal(bool fSignal)
	{
		if (fSignal)
			return Signal();

		return Reset();
	}

	void Wait(DWORD dwMS)
	{
		if (NULL == m_hEvent)
		{
			MYTRACE((LPCWSTR) _T("TEvent::Wait warning: NULL Event.\r\n"));
			return;
		}

		::WaitForSingleObject(m_hEvent, dwMS);
	}

	void Wait()
	{
		Wait(INFINITE);
	}


private:
	HANDLE m_hEvent;
};

#define TEVENT_H
#endif//TEVENT_H
