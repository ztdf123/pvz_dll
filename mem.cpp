#include "pch.h"
#include "mem.h"
void mem::Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldprotect;
	//enable read&write to selected region
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(dst, src, size);
	//restore original protect station
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}
void mem::Nop(BYTE* dst, unsigned int size)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);
	Patch(dst, nopArray, size);
	delete[] nopArray;

}
uintptr_t mem::FindDMAddress(uintptr_t ptr, std::vector <unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++)
	{
		addr = *(uintptr_t*)addr;
		addr = addr + offsets[i];
	}
	return addr;
}