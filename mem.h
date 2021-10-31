#pragma once
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
namespace mem
{
	void Patch(BYTE* dst, BYTE* src, unsigned int size);
	void Nop(BYTE* dst, unsigned int size);
	uintptr_t FindDMAddress(uintptr_t ptr, std::vector <unsigned int> offsets);
}

