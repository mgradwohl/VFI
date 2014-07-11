// QWORD (Quad WORD) 64-bit datatype and helpers
// mattgr
// 9/24/1999

#define QWORD			DWORDLONG
#define LPQWORD			QWORD*

// MAKEx are always MAKEx(low, high)
#define MAKEDWORD(a, b)	((DWORD)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define MAKEQWORD(a, b)	((QWORD)(((DWORD)(a)) | ((QWORD)((DWORD)(b))) << 32))
#define LODWORD(l)		((DWORD)(l))
#define HIDWORD(l)		((DWORD)(((QWORD)(l) >> 32) & 0xFFFFFFFF))
