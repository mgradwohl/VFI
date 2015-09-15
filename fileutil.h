#pragma once
#ifndef FILEUTIL_H

#include <windows.h>
//#include <tchar.h>

#ifdef  __cplusplus
extern "C" {
#endif

bool DoesFileExist(LPCWSTR pszFilePath);
bool DoesFolderExist(LPCWSTR pszFolder);
bool CreateFolder(LPCWSTR pszFolderName);

bool PathGetFolder(LPCWSTR pszFileSpec, LPWSTR pszFolder);
bool PathGetFileName(LPCWSTR pszFileSpec, LPWSTR pszFile);
bool PathIsLocal(LPCWSTR pszPath);
bool PathIsWritable(LPCWSTR pszPath);

bool GetModuleFolder(HINSTANCE hInst, LPWSTR pszFolder);
bool GetModuleName(HINSTANCE hInst, LPWSTR pszName);
bool GetTempFolder(LPWSTR pszFolder);
bool GetWindowsFolder(LPWSTR pszFolder);
bool GetDesktopFolder(LPWSTR pszFolder);
bool GetSystemFolder(LPWSTR pszFolder);
bool GetRecycleFolder(LPWSTR pszFolder);
bool GetDocumentsFolder(LPWSTR pszFolder);
bool GetFontFolder(LPWSTR pszFolder);
bool GetProfileFolder(LPWSTR pszFolder);
bool GetProgramFilesFolder(LPWSTR pszFolder);
bool GetLogFolder(LPCWSTR pszAppname, LPWSTR pszFolder);
bool GetLogFileName(LPCWSTR pszFolder, LPCWSTR pszPrefix, LPCSTR pszPostfix, LPWSTR pszFilename);

bool NukeFolder(LPCWSTR pszFolder);

#ifdef  __cplusplus
}
#endif

#define FILEUTIL_H
#endif//FILEUTIL_H
