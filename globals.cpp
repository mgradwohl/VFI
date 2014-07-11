#include <windows.h>
#include "tevent.h"
//#include "FastHeap.hpp"

DWORD  g_dwThreadCRC = 0;
HANDLE g_hThreadCRC = 0;
TEvent g_eGoThreadCRC;

DWORD  g_dwThreadInfo = 0;
HANDLE g_hThreadInfo = 0;
TEvent g_eGoThreadInfo;


TEvent g_eTermThreads;

BYTE* g_pBuf = NULL;
DWORD g_dwChunk = 0;

