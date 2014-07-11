//
// MyPropPage1.h : header file
//
#ifndef PROPPAGE_H
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
	BOOL	m_fUseISORoot;
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
	virtual int DoModal();
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

#define	 PROPPAGE_H
#endif//PROPPAGE_H
