#ifndef COLUMNINFO_H
#pragma once

#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
	#endif
#endif

class CColumnInfo
{
public:
	CColumnInfo()
	{
		m_dx = 0;
		m_fVisible = true;
		m_idLabel = 0;
		m_iFmt = 0;
		m_dxFixed = 0;
	}

	virtual ~CColumnInfo()
	{
		m_dx = 0;
		m_fVisible = true;
		m_idLabel = 0;
		m_iFmt = 0;
		m_dxFixed = 0;
	}

	bool IsVisible()
		{ return m_fVisible; }
	void SetVisible(bool fVisible)
		{ m_fVisible = fVisible; }
	int GetWidth()
		{ return m_dx; }
	void SetWidth(int iWidth)
		{ m_dx = iWidth; }
	__forceinline int GetFixedWidth()
		{ return m_dxFixed; }
	void SetFixedWidth(int iWidth)
		{ m_dxFixed = iWidth; }
	bool IsFixedWidth()
		{ return m_dxFixed != 0 ;}
	UINT GetLabelID()
		{ return m_idLabel; }
	void SetLabelID(UINT idLabel)
		{ m_idLabel = idLabel; }
	int GetFormat()
		{ return m_iFmt; }
	void SetFormat(UINT iFormat)
		{ m_iFmt = iFormat; }

private:
	UINT	m_idLabel;
	int		m_iFmt;
	int		m_dx;
	int		m_dxFixed;		// non-zero means this is a fixed-width owner draw thing 
	bool	m_fVisible : 1;
};

#define COLUMNINFO_H
#endif//COLUMNINFO_H
