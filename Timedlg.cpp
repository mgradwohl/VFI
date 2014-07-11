// TimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wndlib.h"
#include "VFI.h"
#include "TimeDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static CHAR THIS_FILE[] = __FILE__;
#endif

// DDX Helper functions
void AFXAPI CTimeDlg::DDX_DateTime(CDataExchange* pDX, int nIDC, SYSTEMTIME &SysTime)
{
	HWND hWnd = pDX->m_pDlgWnd->GetDlgItem(nIDC)->GetSafeHwnd();
	
	if (NULL != hWnd && ::IsWindow(hWnd))
	{
		if (pDX->m_bSaveAndValidate)
		{
			::SendMessage(hWnd, DTM_GETSYSTEMTIME, GDT_VALID, (LPARAM)&SysTime);
		}
		else // initializing
		{
			::SendMessage(hWnd, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&SysTime);
		}
	}
}

void AFXAPI CTimeDlg::DDX_Month(CDataExchange* pDX, int nIDC, SYSTEMTIME &SysTime)
{
	HWND hWnd = pDX->m_pDlgWnd->GetDlgItem(nIDC)->GetSafeHwnd();
	
	if (NULL != hWnd && ::IsWindow(hWnd))
	{
		if (pDX->m_bSaveAndValidate)
		{
			::SendMessage(hWnd, MCM_GETCURSEL, GDT_VALID, (LPARAM)&SysTime);
		}
		else // initializing
		{
			::SendMessage(hWnd, MCM_SETCURSEL, GDT_VALID, (LPARAM)&SysTime);
		}
	}
}

void CTimeDlg::DateTimeMerge()
{
	m_stDateTime.wYear=m_stDate.wYear;
	m_stDateTime.wMonth=m_stDate.wMonth;
	m_stDateTime.wDayOfWeek=m_stDate.wDayOfWeek;
	m_stDateTime.wDay=m_stDate.wDay;

	m_stDateTime.wHour=m_stTime.wHour;
	m_stDateTime.wMinute=m_stTime.wMinute;
	m_stDateTime.wSecond=m_stTime.wSecond;
	m_stDateTime.wMilliseconds=m_stTime.wMilliseconds;
}

/////////////////////////////////////////////////////////////////////////////
// CTimeDlg dialog

CTimeDlg::CTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimeDlg)
	//}}AFX_DATA_INIT
}

void CTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeDlg)
	//}}AFX_DATA_MAP
	DDX_Month(pDX, IDC_DATEPICK, m_stDate);
	DDX_DateTime(pDX, IDC_TIMEPICK, m_stTime);
	DateTimeMerge();
}

BEGIN_MESSAGE_MAP(CTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CTimeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeDlg message handlers

BOOL CTimeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//ChangeDialogFont(this, theApp.GetUIFont(), CDF_CENTER);

	//HWND hWnd = GetDlgItem(IDC_DATEPICK)->GetSafeHwnd();
	CRect rc1, rc2;
	CWnd* pWnd = GetDlgItem(IDC_DATEPICK);
	int x = 0;
	if (NULL!=pWnd)
	{
		pWnd->ModifyStyle(0, WS_BORDER, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		GetClientRect(&rc1);
		pWnd->GetWindowRect(&rc2);
		ScreenToClient(&rc2);
		x = (rc1.Width() - rc2.Width()) / 2;
		pWnd->MoveWindow( x, rc2.top, rc2.Width(), rc2.Height(), FALSE);
	}

	//TRACE(L"CTimeDlg::OnOK\tMin Width:%d, Min Height:%d\r\n", rc.right, rc.bottom);
	pWnd = GetDlgItem(IDC_TIMEPICK);
	if (NULL != pWnd)
	{
		pWnd->GetWindowRect(&rc2);
		ScreenToClient(&rc2);
		pWnd->MoveWindow(x, rc2.top, rc2.Width(), rc2.Height(), FALSE);
	}
	return FALSE;
}

void CTimeDlg::GetTime( SYSTEMTIME* pTime )
{
	ASSERT(pTime);
	if (pTime==NULL)
		return;

	UpdateData(TRUE);
	*pTime = m_stDateTime;
}

void CTimeDlg::OnOK() 
{
	UpdateData(TRUE);
#ifdef _DEBUG
	HWND hWnd = GetDlgItem(IDC_DATEPICK)->GetSafeHwnd();
	RECT rc = {0,0,0,0};
	if (NULL!=hWnd)
	{
		::SendMessage(hWnd, MCM_GETMINREQRECT, 0, (LPARAM)&rc);
	}
	TRACE(L"CTimeDlg::OnOK\tMin Width:%d, Min Height:%d\r\n", rc.right, rc.bottom);

#endif

	CDialog::OnOK();
}
