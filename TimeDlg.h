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

#pragma once

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

class CTimeDlg : public CDialog
{
// Construction
public:
	SYSTEMTIME m_stDate;
	SYSTEMTIME m_stTime;
	SYSTEMTIME m_stDateTime;
	CTimeDlg(CWnd* pParent = NULL);   // standard constructor
	void GetTime( SYSTEMTIME* pTime );
	void DateTimeMerge();

// Dialog Data
	//{{AFX_DATA(CTimeDlg)
	enum { IDD = IDD_TIMEBOX };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void AFXAPI DDX_DateTime(CDataExchange* pDX, int nIDC, SYSTEMTIME &SysTime);
	void AFXAPI DDX_Month(CDataExchange* pDX, int nIDC, SYSTEMTIME &SysTime);

	// Generated message map functions
	//{{AFX_MSG(CTimeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
