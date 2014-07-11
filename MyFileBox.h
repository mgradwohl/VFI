#ifndef MYFILEBOX_H
#pragma once
// MyFileBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyFileBox dialog

class CMyFileBox : public CFileDialog
{
	DECLARE_DYNAMIC(CMyFileBox)

public:
	CMyFileBox(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

	bool SetFont(HFONT hFont);

protected:
	//{{AFX_MSG(CMyFileBox)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	HFONT m_hFont;
};

#define MYFILEBOX_H
#endif//MYFILEBOX_H

