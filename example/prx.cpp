
#include "minhook.h"

__declspec (dllexport) void dummy()
{
}

// store a original of funtionToHook
void(*functionToHook_o)();
void hookFunction()
{
	printf("hooked\n");

	// Call the original function
	((void(*)())functionToHook_o)();
}

void functionToHook()
{
	printf("original\n");
}

void* main_start(void*)
{
	// Initialize MinHook.
	if (MH_Initialize() != MH_OK)
	{
		return 0;
	}

	// Hook functionToHook
	if (MH_CreateHook((LPVOID)functionToHook, (LPVOID)hookFunction, reinterpret_cast<void**>(&functionToHook_o)) != MH_OK)
	{
		return 0;
	}

	// Enable the hook for functionToHook
	if (MH_EnableHook((LPVOID)functionToHook) != MH_OK)
	{
		return 0;
	}

	// Call functionToHook
	functionToHook();

	// Disable the hook for functionToHook
	if (MH_DisableHook((LPVOID)functionToHook) != MH_OK)
	{
		return 0;
	}

	// Uninitialize MinHook.
	if (MH_Uninitialize() != MH_OK)
	{
		return 0;
	}

	scePthreadExit(nullptr);
	return nullptr;
}

extern "C" int __cdecl module_start(size_t argc, const void* args)
{
	ScePthread thread;
	scePthreadCreate(&thread, nullptr, main_start, 0, "test thread");
	scePthreadJoin(thread, nullptr);

	return 0;
}

extern "C" int __cdecl module_stop(size_t argc, const void* args)
{
	return 0;
}
