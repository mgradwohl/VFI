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

//
// MyPropPage1.h : header file
//

#pragma once

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

#include "columninfo.h"
//
// CPageGeneral dialog
//
class CPageGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageGeneral)
// Construction
public:
	CPageGeneral();
	~CPageGeneral();

// Dialog Data
	//{{AFX_DATA(CPageGeneral)
	enum { IDD = IDD_GENERALOPTIONS };
	CButton	m_btnBrowse;
	BOOL	m_fAudioCue;
	CString	m_strWave;
	BOOL	m_fIncludePath;
	BOOL	m_fSavePrompt;
	CString	m_strSavePath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageGeneral)
	afx_msg void OnAudiocue();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBrowseWave();
	afx_msg void OnChangeWave();
	afx_msg void OnKillFocusEditWave();
	afx_msg void OnChangeSavepath();
	afx_msg void OnKillfocusSavepath();
	afx_msg void OnBrowsepath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//
//
// CPageColumn dialog
class CPageColumn : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageColumn)

// Construction
public:
	bool m_fInitialized;
	CColumnInfo* GetColumnInfo();
	CColumnInfo* m_pci;
	bool InitCheckList(CColumnInfo* pci);
	CCheckListBox* GetCheckListBox();
	CPageColumn();
	~CPageColumn();

// Dialog Data
	//{{AFX_DATA(CPageColumn)
	enum { IDD = IDD_COLUMNOPTIONS };
	CCheckListBox	m_ctlColumns;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageColumn)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageColumn)
	virtual BOOL OnInitDialog();
	afx_msg void OnClearall();
	afx_msg void OnCheckall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
