#include "windows_wrapper.h"
#include "orbis.h"
#include <mspace.h>

#include <sce_atomic.h>
#include <unordered_map>

std::unordered_map<LPVOID, size_t> allocatedMemory;
size_t size;
void* flexibleMemory = nullptr;

int convert_to_sce_protection(DWORD flProtect)
{
	int protection = 0;
	if (flProtect & PAGE_NOACCESS)
	{
		protection = PROT_NONE;
	}
	else
	{
		if (flProtect & PAGE_READONLY)
		{
			protection |= PROT_READ;
		}
		if (flProtect & PAGE_READWRITE)
		{
			protection |= PROT_READ | PROT_WRITE;
		}
		if (flProtect & PAGE_EXECUTE)
		{
			protection |= PROT_EXEC;
		}
		if (flProtect & PAGE_EXECUTE_READ)
		{
			protection |= PROT_READ | PROT_EXEC;
		}
		if (flProtect & PAGE_EXECUTE_READWRITE)
		{
			protection |= PROT_READ | PROT_WRITE | PROT_EXEC;
		}
	}
	return protection;
}

DWORD convert_to_win_protection(int protection)
{
	DWORD flProtect = 0;
	if (protection & PROT_NONE)
	{
		flProtect = PAGE_NOACCESS;
	}
	else
	{
		if (protection & PROT_READ)
		{
			if (protection & PROT_WRITE)
			{
				flProtect = PAGE_READWRITE;
			}
			else
			{
				flProtect = PAGE_READONLY;
			}
		}
		if (protection & PROT_EXEC)
		{
			if (protection & PROT_READ)
			{
				if (protection & PROT_WRITE)
				{
					flProtect = PAGE_EXECUTE_READWRITE;
				}
				else
				{
					flProtect = PAGE_EXECUTE_READ;
				}
			}
			else
			{
				flProtect = PAGE_EXECUTE;
			}
		}
	}
	return flProtect;
}

SIZE_T QueryMemory(LPVOID lpAddress, LPVOID* start, LPVOID* end, DWORD* flProtect)
{
	int protection;
	void* sstart, *send;

	auto res = sceKernelQueryMemoryProtection(lpAddress, &sstart, &send, &protection);
	if (res < 0)
	{
		return 0;
	}

	if (start)
	{
		*start = sstart;
	}

	if (end)
	{
		*end = send;
	}

	if (flProtect)
	{
		*flProtect = convert_to_win_protection(protection);
	}

	return 1;
}

// wrapper for sceKernelVirtualQuery
SIZE_T VirtualQuery(LPVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength)
{
	// we need to replicate the results of VirtualQuery on windows.
	SceKernelVirtualQueryInfo pageInfo{};
	auto res = sceKernelVirtualQuery(lpAddress, 0, &pageInfo, sizeof(SceKernelVirtualQueryInfo));
	if (res < 0)
	{
		return 0;
	}

	lpBuffer->BaseAddress = pageInfo.start;
	lpBuffer->AllocationBase = reinterpret_cast<PVOID>(pageInfo.start);
	lpBuffer->AllocationProtect = convert_to_win_protection(pageInfo.protection);
	lpBuffer->RegionSize = reinterpret_cast<uint64_t>(pageInfo.end) - reinterpret_cast<uint64_t>(pageInfo.start);
	lpBuffer->State = pageInfo.isCommitted ? MEM_COMMIT : MEM_FREE;
	lpBuffer->Protect = convert_to_win_protection(pageInfo.protection);
	return sizeof(SceKernelVirtualQueryInfo);
}

LPVOID VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
	// we need to replicate the results of VirtualAlloc on windows.
	auto protection = convert_to_sce_protection(flProtect);

	int res = sceKernelMapFlexibleMemory(&lpAddress, dwSize, protection, 0);
	if (res < 0)
	{
		printf("VirualAlloc failed: %X\n", res);
		return NULL;
	}

	allocatedMemory[lpAddress] = dwSize;

	return lpAddress;
}

BOOL VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType)
{
	auto it = allocatedMemory.find(lpAddress);
	if (it == allocatedMemory.end())
	{
		printf("VirtualFree failed: memory not found\n");
		return FALSE;
	}

	// we need to replicate the results of VirtualFree on windows.
	int res = sceKernelMunmap(lpAddress, (*it).second);
	if (res < 0)
	{
		printf("sceKernelMunmap failed: %d\n", res);
		return FALSE;
	}

	return TRUE;
}

BOOL VirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
	// we need to replicate the results of VirtualProtect on windows.

	int oldProt = 0;
	int res = sceKernelQueryMemoryProtection(lpAddress, 0, 0, &oldProt);
	if (res < 0)
	{
		return FALSE;
	}

	if (lpflOldProtect)
		*lpflOldProtect = convert_to_win_protection(oldProt);

	auto newProt = convert_to_sce_protection(flNewProtect);
	res = sceKernelMprotect(lpAddress, dwSize, newProt);
	if (res < 0)
	{
		return FALSE;
	}
	return TRUE;
}

VOID GetSystemInfo(LPSYSTEM_INFO lpSystemInfo)
{
	if (lpSystemInfo == nullptr)
		return;

	lpSystemInfo->lpMinimumApplicationAddress = reinterpret_cast<PVOID>(0x10000);
	lpSystemInfo->lpMaximumApplicationAddress = reinterpret_cast<PVOID>(0x00007FFFFFFFFFFF);
	lpSystemInfo->dwPageSize = PAGE_SIZE;
	lpSystemInfo->dwNumberOfProcessors = sceKernelGetCpumode();
	lpSystemInfo->dwAllocationGranularity = 0x10000;
}

HANDLE HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
{
	// we need to allocate at least one page
	if (dwInitialSize == 0)
		dwInitialSize = PAGE_SIZE;

	// create direct memory pool
	void* addr = nullptr;
	if (int res = sceKernelMmap(0, dwInitialSize, PROT_READ | PROT_WRITE, 0x1000 | 0x2, -1, 0, &addr) < 0)
	{
		printf("sceKernelMapFlexibleMemory failed: %X\n", res);
		return nullptr;
	}

	flexibleMemory = addr;

	SceLibcMspace mspace = sceLibcMspaceCreate("heap", addr, dwInitialSize, 0);
	if (mspace == nullptr)
	{
		printf("sceLibcMspaceCreate failed\n");
		return nullptr;
	}

	size = dwInitialSize;

	return mspace;
}

BOOL HeapDestroy(HANDLE hHeap)
{
	if (int res = sceLibcMspaceDestroy(hHeap) < 0)
	{
		printf("sceLibcMspaceDestroy failed: %X\n", res);
		return FALSE;
	}


	if (int res = sceKernelMunmap(flexibleMemory, size) < 0)
	{
		printf("sceKernelMunmap failed: %X\n", res);
		return FALSE;
	}
	return TRUE;
}


LPVOID HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes)
{
	void* addr = sceLibcMspaceMalloc((SceLibcMspace)hHeap, dwBytes);
	if (addr == nullptr)
	{
		return nullptr;
	}
	return addr;
}

BOOL HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
	if (int res = sceLibcMspaceFree(hHeap, lpMem) < 0)
	{
		printf("sceLibcMspaceFree failed: %X\n", res);
		return FALSE;
	}

	return TRUE;
}

LPVOID HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes)
{
	if (lpMem == nullptr)
	{
		// If the old pointer is null, behave like HeapAlloc
		return sceLibcMspaceMalloc((SceLibcMspace)hHeap, dwBytes);
	}

	if (dwBytes == 0)
	{
		// If the new size is zero, behave like HeapFree
		sceLibcMspaceFree((SceLibcMspace)hHeap, lpMem);
		return nullptr;
	}

	// Retrieve the size of the old memory block using stats
	SceLibcMallocManagedSize stats;
	if (sceLibcMspaceMallocStats((SceLibcMspace)hHeap, &stats) < 0)
	{
		printf("sceLibcMspaceMallocStats failed.\n");
		return nullptr;
	}

	// Assume stats can somehow provide the size of the specific block (replace with real logic)
	size_t oldSize = stats.size;
	if (oldSize == 0)
	{
		printf("Failed to get old block size.\n");
		return nullptr;
	}

	// Allocate new memory with the requested size
	void* newAddr = sceLibcMspaceMalloc((SceLibcMspace)hHeap, dwBytes);
	if (newAddr == nullptr)
	{
		printf("sceLibcMspaceMalloc failed for reallocation.\n");
		return nullptr;
	}

	// Copy data from the old block to the new block
	memcpy(newAddr, lpMem, oldSize < dwBytes ? oldSize : dwBytes);

	// Free the old block
	sceLibcMspaceFree((SceLibcMspace)hHeap, lpMem);

	return newAddr;
}

// GetThreadContext
BOOL GetThreadContext(HANDLE hThread, LPCONTEXT lpContext)
{
	SceDbgUcontext ucontext;
	if(pthread_get_user_context_np((thread*)hThread, &ucontext) < 0)
		return FALSE;

	lpContext->Rdi = ucontext.uc_mcontext.mc_rdi;
	lpContext->Rsi = ucontext.uc_mcontext.mc_rsi;
	lpContext->Rdx = ucontext.uc_mcontext.mc_rdx;
	lpContext->Rcx = ucontext.uc_mcontext.mc_rcx;
	lpContext->Rax = ucontext.uc_mcontext.mc_rax;
	lpContext->Rbx = ucontext.uc_mcontext.mc_rbx;
	lpContext->Rbp = ucontext.uc_mcontext.mc_rbp;
	lpContext->Rsp = ucontext.uc_mcontext.mc_rsp;
	lpContext->Rip = ucontext.uc_mcontext.mc_rip;

	lpContext->R8 = ucontext.uc_mcontext.mc_r8;
	lpContext->R9 = ucontext.uc_mcontext.mc_r9;
	lpContext->R10 = ucontext.uc_mcontext.mc_r10;
	lpContext->R11 = ucontext.uc_mcontext.mc_r11;
	lpContext->R12 = ucontext.uc_mcontext.mc_r12;
	lpContext->R13 = ucontext.uc_mcontext.mc_r13;
	lpContext->R14 = ucontext.uc_mcontext.mc_r14;
	lpContext->R15 = ucontext.uc_mcontext.mc_r15;

	return TRUE;
}

// SetThreadContext
BOOL SetThreadContext(HANDLE hThread, const CONTEXT* lpContext)
{
	SceDbgUcontext ucontext;
	if(pthread_get_user_context_np((thread*)hThread, &ucontext) < 0)
		return FALSE;

	ucontext.uc_mcontext.mc_rdi = lpContext->Rdi;
	ucontext.uc_mcontext.mc_rsi = lpContext->Rsi;
	ucontext.uc_mcontext.mc_rdx = lpContext->Rdx;
	ucontext.uc_mcontext.mc_rcx = lpContext->Rcx;
	ucontext.uc_mcontext.mc_rax = lpContext->Rax;
	ucontext.uc_mcontext.mc_rbx = lpContext->Rbx;
	ucontext.uc_mcontext.mc_rbp = lpContext->Rbp;
	ucontext.uc_mcontext.mc_rsp = lpContext->Rsp;
	ucontext.uc_mcontext.mc_rip = lpContext->Rip;

	ucontext.uc_mcontext.mc_r8 = lpContext->R8;
	ucontext.uc_mcontext.mc_r9 = lpContext->R9;
	ucontext.uc_mcontext.mc_r10 = lpContext->R10;
	ucontext.uc_mcontext.mc_r11 = lpContext->R11;
	ucontext.uc_mcontext.mc_r12 = lpContext->R12;
	ucontext.uc_mcontext.mc_r13 = lpContext->R13;
	ucontext.uc_mcontext.mc_r14 = lpContext->R14;
	ucontext.uc_mcontext.mc_r15 = lpContext->R15;

	if (pthread_set_user_context_np((thread*)hThread, &ucontext) < 0)
		return FALSE;

	return TRUE;
}

HANDLE CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID)
{
	return HANDLE();
}

BOOL Thread32First(HANDLE hSnapshot, LPTHREADENTRY32 lpte)
{
	return FALSE;
}

DWORD GetCurrentProcessId()
{
	return getpid();
}

DWORD GetCurrentThreadId()
{
	return ((thread*)scePthreadSelf())->tid;
}

HANDLE OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId)
{
	auto libkernel = lpLibkernelBase;
	if (libkernel == 0)
	{
		printf("Failed to get libkernel.sprx address\n");
		return 0;
	}

	auto currentThread = *reinterpret_cast<thread**>(GetAbsoluteAddress(0x0058248, libkernel));
	while (currentThread != nullptr)
	{
		if (currentThread->tid == dwThreadId)
			return currentThread;

		currentThread = currentThread->next;
	}

	return 0;
}

// SuspendThread
DWORD SuspendThread(HANDLE hThread)
{
	// this is bugged & causes a deadlock
	//if (pthread_suspend_user_context_np((thread*)hThread) < 0)
	//	return -1;
	//return 0;

	return -1; 
}

// ResumeThread
DWORD ResumeThread(HANDLE hThread)
{
	if (pthread_resume_user_context_np((thread*)hThread) < 0)
		return -1;
	return 0;
}

// CloseHandle
BOOL CloseHandle(HANDLE hObject)
{
	return TRUE;
}

BOOL FlushInstructionCache(HANDLE hProcess, LPCVOID lpBaseAddress, SIZE_T dwSize)
{
	// how do i flush instruction cache on orbis?
	// we need to replicate the results of FlushInstructionCache on windows.

	return TRUE;
}

HANDLE GetCurrentProcess()
{
	return 0;
}

VOID Sleep(DWORD dwMilliseconds)
{
	// use sceKernelUsleep
	sceKernelUsleep(dwMilliseconds * 1000);
}

LONG InterlockedCompareExchange(LONG volatile* Destination, LONG Exchange, LONG Comparand)
{
	return sceAtomicCompareAndSwap32((volatile int32_t*)Destination, Comparand, Exchange);
}

LONG InterlockedExchange(LONG volatile* Target, LONG Value)
{
	return sceAtomicExchange32((volatile int32_t*)Target, Value);
}

HMODULE GetModuleHandleW(LPCWSTR lpModuleName)
{
	return reinterpret_cast<HMODULE>(sceKernelLoadStartModule(lpModuleName, 0, nullptr, 0, nullptr, nullptr));
}

LPVOID GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{ 
	uint32_t module = *(uint32_t*)&hModule;

	return (void*)GetExport((int)module, lpProcName);
}
