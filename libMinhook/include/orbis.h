#pragma once

template<typename T = uintptr_t>
T GetAbsoluteAddress(uintptr_t address, uintptr_t base = -1)
{
	return reinterpret_cast<T>((base == -1 ? 0x400000 : base) + address);
}

template<typename T = uintptr_t>
T GetRelativeAddress(uintptr_t address, uintptr_t base)
{
	return reinterpret_cast<T>(address - base);
}

template<typename T = uintptr_t>
T GetModuleAddress(std::string module)
{
	int library = sceKernelLoadStartModule(module.data(), 0, nullptr, 0, nullptr, nullptr);
	if (library > 0)
	{
		OrbisKernelModuleInfo moduleInfo;
		moduleInfo.size = sizeof(OrbisKernelModuleInfo);
		if (sceKernelGetModuleInfo(library, &moduleInfo) == 0)
		{
			return reinterpret_cast<T>(moduleInfo.segmentInfo[0].address);
		}
	}

	return {};
}	

template<typename T = uintptr_t>
T GetExport(int handle, std::string function)
{
	uint64_t libraryFunctionAddr;
	if (sceKernelDlsym(handle, function.data(), (void**)&libraryFunctionAddr) == 0)
	{
		return (T)libraryFunctionAddr;
	}
	else
		return 0;
}

thread* GetByName(const char* name);
thread* GetById(uint32_t id);
