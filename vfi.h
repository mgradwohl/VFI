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

#ifndef VFI_H
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sysinfo.h"
#include "tevent.h"
#include "strlib.h"

#include "resource.h"
#include "wisefile.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

void ChangeDialogFont(CWnd* pWnd, CFont* pFont, int nFlag);
/////////////////////////////////////////////////////////////////////////////
// CMyApp:
// See GameCRC.cpp for the implementation of this class
//
class CMyApp : public CWinApp
{
public:
	inline UINT WM_UPDATEVIEW()
	{
		return m_wmUpdateView;
	}

	CFont* GetUIFont();
	CString m_strISORoot;
	WCHAR m_szIniPath[MAX_PATH];
	bool ForwardMessages();
	CMyApp();
	BOOL FirstInstance();
	BOOL SetFocusToPrevInstance();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMyApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	TEvent m_eOneTime;
private:
	UINT m_wmUpdateView;
};

/////////////////////////////////////////////////////////////////////////////
extern CMyApp theApp;

#define VFI_H
#endif//VFI_H