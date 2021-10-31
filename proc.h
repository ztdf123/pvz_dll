#pragma once

#include <vector>
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcId(const wchar_t* procName);
uintptr_t GetMoudleBaseAddress(DWORD procId, const wchar_t* modName);
uintptr_t FindDMAddress(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);




