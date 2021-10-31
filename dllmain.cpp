// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "proc.h"
#include "mem.h"
// Created with ReClass.NET 1.2 by KN4CK3R

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

DWORD procId = GetProcId(L"PlantsVsZombies.exe");
DWORD moduleBase = GetMoudleBaseAddress(procId, L"PlantsVsZombies.exe");

/* incSolarHook incSolar
PlantsVsZombies.exe+1BA74 - 2B F3                 - sub esi,ebx
PlantsVsZombies.exe+1BA76 - 89 B7 60550000        - mov [edi+00005560],esi
*/

/* bigSolarHook bigSolar
 PlantsVsZombies.exe+30A11 - 01 88 60550000        - add [eax+00005560],ecx
*/
class hookent {
public:
    int hookLenth;
    DWORD hookAddress;
    DWORD jmpBackAddress;
    BYTE stolenBytes[30];
    bool bhook = false;
};

hookent incSolarHook, bigSolarHook;

bool Hook(void* toHook, void* ourFunct, int len) {

    //the hook size must over 5
    if (len < 5) {
        return false;
    }
    //1.find where to hook and nop the original operates
    DWORD curProtection;
    
    VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

    memset(toHook, 0x90, len);

    //2.cauculate the relative adrress from 
    //  then write {jmp ourfunc}(asm,which occupy 5 bytes in binary)

    DWORD relativeAddress = (DWORD)ourFunct - ((DWORD)toHook + 5);


    *(BYTE*)toHook = 0xE9;
    *(DWORD*)((DWORD)toHook + 1) = relativeAddress;

    //3.restore the old protection
    DWORD temp;
    VirtualProtect(toHook, len, curProtection, &temp);

    return true; 
}

void __declspec(naked) incSolar(){
    __asm {
        add esi, ebx
        mov[edi + 0x5560], esi
        jmp incSolarHook.jmpBackAddress
    }

}

void __declspec(naked) bigSolar() {
    __asm {
        mov ecx, 0x64
        add[eax + 0x5560], ecx
        jmp bigSolarHook.jmpBackAddress
    }
}

DWORD WINAPI MainThread(HMODULE hModule)
{
    AllocConsole();
    FILE* tmp;
    freopen_s(&tmp, "CONOUT$", "w", stdout);
    std::cout << "Welcom to my cheat\nCreated by Antares\n";
    
    incSolarHook.hookLenth = 8;
    incSolarHook.hookAddress = moduleBase + 0x1BA74;
    incSolarHook.jmpBackAddress = incSolarHook.hookAddress + incSolarHook.hookLenth;
    memcpy_s(incSolarHook.stolenBytes, sizeof(incSolarHook.stolenBytes), "\x2B\xF3\x89\xB7\x60\x55\x00\x00", 8);
    

    bigSolarHook.hookLenth = 6;
    bigSolarHook.hookAddress = moduleBase + 0x30A11;
    bigSolarHook.jmpBackAddress = bigSolarHook.hookAddress + bigSolarHook.hookLenth;
    memcpy_s(bigSolarHook.stolenBytes, sizeof(bigSolarHook.stolenBytes),"\x01\x88\x60\x55\x00\x00", 6);
    

    while (true) {
        if (GetAsyncKeyState(VK_NUMPAD1) & 1){
            incSolarHook.bhook = !incSolarHook.bhook;
            if (incSolarHook.bhook) {
                Hook((void*)incSolarHook.hookAddress, incSolar, incSolarHook.hookLenth);
                std::cout << "incSolar enabled!\n";
            }
            else {
//              mem::Patch((BYTE*)incSolarHook.hookAddress, (BYTE*)"\x2B\xF3\x89\xB7\x60\x55\x00\x00", 8);
                mem::Patch((BYTE*)incSolarHook.hookAddress, incSolarHook.stolenBytes, 8);               
                std::cout << "incSolar disabled!\n";
            }
        }
        if (GetAsyncKeyState(VK_NUMPAD2) & 1){
            bigSolarHook.bhook = !bigSolarHook.bhook;
            if (bigSolarHook.bhook) {
                Hook((void*)bigSolarHook.hookAddress, bigSolar, bigSolarHook.hookLenth);
                std::cout << "bigSolar enabled!\n";
            }
            else {
                //mem::Patch((BYTE*)bigSolarHook.hookAddress, (BYTE*)"\x01\x88\x60\x55\x00\x00", 6);
                mem::Patch((BYTE*)bigSolarHook.hookAddress, bigSolarHook.stolenBytes, 6);
                std::cout << "bigSolar disabled!\n";
            }
        }
        if (GetAsyncKeyState(VK_END) & 1){   
            break;
        }
        Sleep(5);
    }
    fclose(tmp);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0);
        break;
    }
    return TRUE;
}


