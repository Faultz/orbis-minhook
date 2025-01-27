#include <orbis/libkernel.h>

#include <minhook.h>

void (*functionToHook_o)();

void hookFunction()
{
  sceKernelDebugOutText(0, "function hooked: functionToHook()->hookFunction()");

  ((void (*)())functionToHook_o)();
}

void functionToHook()
{
  sceKernelDebugOutText(0, "original function with no hooking: functionToHook()");
}

void *main_start(void *)
{
  // Initialize MinHook.
  if (MH_Initialize() != MH_OK)
    return 0;

  // Hook functionToHook
  if (MH_CreateHook((LPVOID)functionToHook, (LPVOID)hookFunction, reinterpret_cast<void **>(&functionToHook_o)) != MH_OK)
    return 0;

  // Enable the hook for functionToHook
  if (MH_EnableHook((LPVOID)functionToHook) != MH_OK)
    return 0;

  // Call functionToHook
  functionToHook();

  // Disable the hook for functionToHook
  if (MH_DisableHook((LPVOID)functionToHook) != MH_OK)
    return 0;

  // Uninitialize MinHook.
  if (MH_Uninitialize() != MH_OK)
    return 0;

  pthread_exit(nullptr);

  return nullptr;
}

extern "C" int32_t __wrap__init(size_t args, const void *argp)
{
  pthread_t thread;

  // Create and run the thread
  pthread_create(&thread, nullptr, main_start, nullptr);
  pthread_join(thread, nullptr);

  // Initialize function hook (hooking logic should be placed here)
  functionToHook_o = functionToHook; // Save the original function address
  hookFunction();                    // Call the hooked function

  return 0;
}
