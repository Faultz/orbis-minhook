#pragma once

/*
 * This file is wrapper for all the windows specific types and functions to be used in the official orbis sdk.
 */

#define REG_RDI  0
#define REG_RSI  1
#define REG_RDX  2
#define REG_RCX  3
#define REG_RAX  4
#define REG_RBX  5
#define REG_RBP  6
#define REG_RSP  7
#define REG_RIP  8
#define REG_R8   9
#define REG_R9   10
#define REG_R10  11
#define REG_R11  12
#define REG_R12  13
#define REG_R13  14
#define REG_R14  15
#define REG_R15  16

typedef long long INT64;
typedef unsigned short UINT16;
typedef unsigned long long UINT64;
typedef void *LPVOID;
typedef const void *LPCVOID;
typedef void VOID;
typedef const char *LPCWSTR;
typedef const char *LPCSTR;
typedef unsigned char UINT8;
typedef unsigned int UINT;
typedef bool BOOL;
typedef unsigned long long DWORD;
typedef unsigned long long ULONG_PTR;
typedef ULONG_PTR DWORD_PTR;
typedef void *PVOID;
typedef int WORD;
typedef size_t SIZE_T;
typedef DWORD *LPDWORD;
typedef unsigned char BYTE;
typedef char CCHAR;
typedef char CHAR;
typedef BYTE *LPBYTE;
typedef unsigned int UINT32;
typedef int INT32;
typedef int8_t INT8;
typedef int16_t INT16;
typedef UINT32 *PUINT32;
typedef void *HANDLE;
typedef long LONG;
typedef DWORD *PDWORD;
typedef unsigned long long DWORD64;
typedef unsigned long long ULONG64;
typedef HANDLE HINSTANCE;
typedef HINSTANCE HMODULE;

#define CONST const;
#define FALSE 0
#define TRUE 1

typedef struct _MEMORY_BASIC_INFORMATION
{
	PVOID BaseAddress;
	PVOID AllocationBase;
	DWORD AllocationProtect;
	SIZE_T RegionSize;
	DWORD State;
	DWORD Protect;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

#define MEM_COMMIT 0x1000
#define MEM_FREE 0x10000
#define MEM_RESERVE 0x2000
#define MEM_RELEASE 0x00008000

#define PAGE_NOACCESS 0x01
#define PAGE_READONLY 0x02
#define PAGE_READWRITE 0x04
#define PAGE_WRITECOPY 0x08
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_EXECUTE_READ 0x20
#define PAGE_EXECUTE 0x10
#define PAGE_EXECUTE_WRITECOPY 0x80

typedef struct _SYSTEM_INFO
{
	DWORD dwPageSize;
	LPVOID lpMinimumApplicationAddress;
	LPVOID lpMaximumApplicationAddress;
	DWORD dwNumberOfProcessors;
	DWORD dwAllocationGranularity;
} SYSTEM_INFO, *LPSYSTEM_INFO;

typedef struct _PROCESS_INFORMATION
{
	HANDLE hProcess;
	HANDLE hThread;
	DWORD dwProcessId;
	DWORD dwThreadId;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION;

typedef struct _STARTUPINFO
{
	DWORD cb;
	LPCSTR lpReserved;
	LPCSTR lpDesktop;
	LPCSTR lpTitle;
	DWORD dwX;
	DWORD dwY;
	DWORD dwXSize;
	DWORD dwYSize;
	DWORD dwXCountChars;
	DWORD dwYCountChars;
	DWORD dwFillAttribute;
	DWORD dwFlags;
	WORD wShowWindow;
	WORD cbReserved2;
	LPBYTE lpReserved2;
	HANDLE hStdInput;
	HANDLE hStdOutput;
	HANDLE hStdError;
} STARTUPINFO, *LPSTARTUPINFO;

typedef struct _SECURITY_ATTRIBUTES
{
	DWORD nLength;
	LPVOID lpSecurityDescriptor;
	BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES;

/*	__register_t	mc_onstack;
		__register_t	mc_rdi;
		__register_t	mc_rsi;
		__register_t	mc_rdx;
		__register_t	mc_rcx;
		__register_t	mc_r8;
		__register_t	mc_r9;
		__register_t	mc_rax;
		__register_t	mc_rbx;
		__register_t	mc_rbp;
		__register_t	mc_r10;
		__register_t	mc_r11;
		__register_t	mc_r12;
		__register_t	mc_r13;
		__register_t	mc_r14;
		__register_t	mc_r15;
		__uint32_t	mc_trapno;
		__uint16_t	mc_fs;
		__uint16_t	mc_gs;
		__register_t	mc_addr;
		__uint32_t	mc_flags;
		__uint16_t	mc_es;
		__uint16_t	mc_ds;
		__register_t	mc_err;
		__register_t	mc_rip;
		__register_t	mc_cs;
		__register_t	mc_rflags;
		__register_t	mc_rsp;
		__register_t	mc_ss;
		long	mc_len;
		long	mc_fpformat;
		long	mc_ownedfp;
		long	mc_fpstate[104] __aligned(64);
		__register_t	mc_fsbase;
		__register_t	mc_gsbase;
		long	mc_spare[6];
		*/

// keep format of windows types but with types above
typedef struct _CONTEXT
{
	UINT64 Onstack;
	UINT64 Rdi;
	UINT64 Rsi;
	UINT64 Rdx;
	UINT64 Rcx;
	UINT64 R8;
	UINT64 R9;
	UINT64 Rax;
	UINT64 Rbx;
	UINT64 Rbp;
	UINT64 R10;
	UINT64 R11;
	UINT64 R12;
	UINT64 R13;
	UINT64 R14;
	UINT64 R15;
	UINT32 Trapno;
	UINT16 Fs;
	UINT16 Gs;
	UINT64 Addr;
	UINT32 Flags;
	UINT16 Es;
	UINT16 Ds;
	UINT64 Err;
	UINT64 Rip;
	UINT64 Cs;
	UINT64 Rflags;
	UINT64 Rsp;
	UINT64 Ss;
} CONTEXT, *PCONTEXT;

typedef CONTEXT *LPCONTEXT;

typedef struct tagTHREADENTRY32
{
	DWORD dwSize;
	DWORD cntUsage;
	DWORD th32ThreadID;
	DWORD th32OwnerProcessID;
	LONG tpBasePri;
	LONG tpDeltaPri;
	DWORD dwFlags;
} THREADENTRY32;

// LPTHREADENTRY32
typedef THREADENTRY32 *LPTHREADENTRY32;

static UINT64 lpLibkernelBase;

SIZE_T QueryMemory(LPVOID lpAddress, LPVOID *start, LPVOID *end, DWORD *flProtect);
SIZE_T VirtualQuery(LPVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
LPVOID VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
BOOL VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
BOOL VirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
VOID GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);

LPVOID HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
BOOL HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);

// HeapReAlloc
LPVOID HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes);

// HeapCreate
HANDLE HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);

// HeapDestroy
BOOL HeapDestroy(HANDLE hHeap);

// GetThreadContext
BOOL GetThreadContext(HANDLE hThread, LPCONTEXT lpContext);

// SetThreadContext
BOOL SetThreadContext(HANDLE hThread, const CONTEXT *lpContext);

// CreateToolhelp32Snapshot
HANDLE CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID);

// Thread32First
BOOL Thread32First(HANDLE hSnapshot, LPTHREADENTRY32 lpte);

// GetCurrentProcessId
DWORD GetCurrentProcessId();

// GetCurrentThreadId
DWORD GetCurrentThreadId();

// OpenThread
HANDLE OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId);

// SuspendThread
DWORD SuspendThread(HANDLE hThread);

// ResumeThread
DWORD ResumeThread(HANDLE hThread);

// CloseHandle
BOOL CloseHandle(HANDLE hObject);

// FlushInstructionCache
BOOL FlushInstructionCache(HANDLE hProcess, LPCVOID lpBaseAddress, SIZE_T dwSize);

// GetCurrentProcess
HANDLE GetCurrentProcess();

// Sleep
VOID Sleep(DWORD dwMilliseconds);

// InterlockedCompareExchange
LONG InterlockedCompareExchange(LONG volatile *Destination, LONG Exchange, LONG Comparand);

// InterlockedExchange
LONG InterlockedExchange(LONG volatile *Target, LONG Value);

// GetModuleHandleW
HMODULE GetModuleHandleW(LPCWSTR lpModuleName);

// GetProcAddress
LPVOID GetProcAddress(HMODULE hModule, LPCSTR lpProcName);