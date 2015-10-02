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

#define ISO_UNKNOWN				0x0000
#define ISO_NONE				0x0000
#define ISO_START				0x0001
#define ISO_EXT_TOOLONG			0x0001
#define ISO_EXT_BADCHAR			0x0002
#define ISO_EXT_HASDOT			0x0004
#define ISO_EXT_LOWERCASE		0x0008
#define ISO_FILE_TOOLONG		0x0010
#define ISO_FILE_BADCHAR		0x0020
#define ISO_FILE_HASDOT			0x0040
#define ISO_FILE_LOWERCASE		0x0080
#define ISO_PATH_TOOLONG		0x0100
#define ISO_PATH_TOODEEP		0x0200
#define ISO_PATH_BADCHAR		0x0400
#define ISO_PATH_HASDOT			0x0800
#define ISO_PATH_LOWERCASE		0x1000
#define ISO_END					0x1000
#define ISO_MAX					0x1FFF

WORD isoGetISOFlags(LPCWSTR wzPath);
WORD isoGetISOFlags(LPCWSTR wzPath, LPCWSTR wzName, LPCWSTR wzExt);
LPWSTR isoGetISOFlagsString(const WORD wISOFlags);
