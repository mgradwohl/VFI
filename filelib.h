// filelib.h
// mattgr
// 3/27/2000

#pragma once
#ifndef FILELIB_H

#ifndef __cplusplus
	#error FileLib requires C++ compilation (use a .cpp suffix)
#endif

//#ifndef _LIB
//		#ifdef _DEBUG
//			#pragma comment(lib, "filelibdu.lib")
//		#else
//			#pragma comment(lib, "filelibu.lib")
//		#endif
//#endif//_LIB

#include <windows.h>

bool CreateFolder(LPCWSTR pszFolder);
bool GetTempFolder(LPWSTR pszFolder);
bool GetWindowsFolder(LPWSTR pszFolder);
bool GetDesktopFolder(LPWSTR pszFolder);
bool GetSystemFolder(LPWSTR pszFolder);
bool GetRecycleFolder(LPWSTR pszFolder);
bool GetDocumentsFolder(LPWSTR pszFolder);
bool GetFontFolder(LPWSTR pszFolder);
bool GetProfileFolder(LPWSTR pszFolder);
bool GetProgramFilesFolder(LPWSTR pszFolder);
bool GetVolumeLabel(LPCWSTR pszRootPathName, LPWSTR pszVolumeName, DWORD cchVolumeName);
bool DoesFileExist(LPCWSTR pszFileName);
bool DoesFolderExist(LPCWSTR pszFolder);
bool PathGetFolder(LPWSTR pszFileSpec);
bool PathGetFileName(LPWSTR pszFileSpec);
bool GetModuleFolder(HINSTANCE hInst, LPWSTR pszFolder);
bool GetLogFolder(LPCWSTR pszAppname, LPWSTR pszFolder);
bool GetLogFileName(LPCWSTR pszFolder, LPCWSTR pszPrefix, LPCWSTR pszPostfix, LPWSTR pszFilename, LPWSTR pszExt);
bool GetModuleName(HINSTANCE hInst, LPWSTR pszName);
bool PathIsLocal(LPCWSTR pszPath);
bool PathIsWritable(LPCWSTR pszPath);
bool PathGetLongName(LPCWSTR lpszShortPath, LPWSTR lpszLongPath);
bool PathIsRootOnly(LPCWSTR pszPath);
LPWSTR PathGetRootName(LPWSTR pszPath);
bool PathGetContainingFolder(LPWSTR pszFileSpec);
__int64 SetFilePointer64 (HANDLE hf, __int64 distance, DWORD MoveMethod);
__int64 GetFileSize64(LPCWSTR pszFileSpec);
void ShowShellFileProperties(HWND hwnd, LPCWSTR pwszFile);

#define FILELIB_H
#endif//FILELIB_H
