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

#ifndef __cplusplus
	#error FileLib requires C++ compilation (use a .cpp suffix)
#endif

#include "strlib.h"

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
