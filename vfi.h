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
//	bool GetCommandLineFolder(LPWSTR pszFolder);
	bool CheckCommonControls();
	inline UINT WM_UPDATEVIEW()
	{
		return m_wmUpdateView;
	}

	CFont* GetUIFont();
	CString m_strISORoot;
//	int ErrorMsgBox( CWnd* pWnd, CString strMessage, UINT uStyle, DWORD dwError);
	WCHAR m_szIniPath[MAX_PATH];
//	CString GetModuleDirectory();
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

