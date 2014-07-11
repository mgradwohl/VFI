#ifndef FILEVERSION_H
#pragma once

#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
	#endif
#endif

class CFileVersion  
{
public:
	CFileVersion();
	virtual ~CFileVersion();

	int GetMajor()			{ return m_V1;}
	int GetMinor()			{ return m_V2;}
	int GetBuild()			{ return m_V3;}

	int GetV1()				{return m_V1;}
	int GetV2()				{return m_V2;}
	int GetV3()				{return m_V3;}
	int GetV4()				{return m_V4;}

	bool GetVersion3(LPWSTR pszVersion);
	bool GetVersion4(LPWSTR pszVersion);
	bool Open(LPCWSTR pszFile);

private:
	bool ReadVersionInfo();
	int m_V1;
	int m_V2;
	int m_V3;
	int m_V4;
	bool m_fRead;
	LPWSTR m_pszFile;
};

#define FILEVERSION_H
#endif//FILEVERSION_H
