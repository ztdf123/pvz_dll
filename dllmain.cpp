﻿// dllmain.cpp : 定义 DLL 应用程序的入口点。
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
class patchent {
public:
    int len;
    DWORD dst;
    bool bPatch = false;
    BYTE stolenBytes[30];
    BYTE patchBytes[30];
    void restore() {
        mem::Patch((BYTE*)dst, stolenBytes, len);
    }
    void patch() {
        mem::Patch((BYTE*)dst, patchBytes, len);
    }
    void nop() {
        mem::Nop((BYTE*)dst, len);
    }
    void backup() {
        memcpy_s(stolenBytes, len, (void*)dst, len);
    }
    void setPatchBytes(const char* hcode) {
        memcpy_s(patchBytes, len, hcode, len);
    }
};


class hookent {
public:
    int hookLenth;
    DWORD hookAddress;
    DWORD jmpBackAddress;
    BYTE stolenBytes[30];
    bool bHook = false;
    void restore() {
        mem::Patch((BYTE*)hookAddress, stolenBytes, hookLenth);
    }
    bool Hook(void* ourFunct) {
        void* toHook = (void*)hookAddress;
        int len = hookLenth;
    //the hook size must over 5
    if (len < 5) {
        return false;
    }
    //1.find where to hook and nop the original operates
        DWORD curProtection = 0;
    
    VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

    memset(toHook, 0x90, len);

        //2.cauculate the relative adrress from hookAddress to ourFunctinon address
        // then write our jmp code
        // {jmp ourfunc}(asm,which occupy 5 bytes in binary)

    DWORD relativeAddress = (DWORD)ourFunct - ((DWORD)toHook + 5);


        *(BYTE*)toHook = 0xE9; //asm:jmp
    *(DWORD*)((DWORD)toHook + 1) = relativeAddress;

    //3.restore the old protection
    VirtualProtect(toHook, len, curProtection, &curProtection);

    return true; 
    }
};
hookent incSolarHook, bigSolarHook;
patchent invinPlantes, secKill, noCd, autoPick, ignoreArmor, backRun;

void __declspec(naked) incSolar() {
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



void helpInfo()
{
    AllocConsole();
    FILE* tmp;
    freopen_s(&tmp, "CONOUT$", "w", stdout);
    std::cout << "Welcom to my cheat\nCreated by Antares\n";
    std::cout << "press 0 to inc 1000 solar\npress 1 to incsolar when plants\npress 2 to enbale bigsolar\n";
    std::cout << "press 3 to invinPlants\npress 4 to secKill\npress 5 to noCd\n";
    std::cout << "press home to backRun\n";
}
void init()
{
    incSolarHook.hookLenth = 8;
    incSolarHook.hookAddress = moduleBase + 0x1BA74;
    incSolarHook.jmpBackAddress = incSolarHook.hookAddress + incSolarHook.hookLenth;
    memcpy_s(incSolarHook.stolenBytes, incSolarHook.hookLenth, (void*)incSolarHook.hookAddress, incSolarHook.hookLenth);
    

    bigSolarHook.hookLenth = 6;
    bigSolarHook.hookAddress = moduleBase + 0x30A11;
    bigSolarHook.jmpBackAddress = bigSolarHook.hookAddress + bigSolarHook.hookLenth;
    memcpy_s(bigSolarHook.stolenBytes, bigSolarHook.hookLenth, (void*)bigSolarHook.hookAddress, bigSolarHook.hookLenth);

    invinPlantes.len = 4;
    invinPlantes.dst = moduleBase + 0x12FCF0;
    invinPlantes.backup();
    invinPlantes.setPatchBytes("\x83\x46\x40\x04");

    ignoreArmor.len = 2;
    ignoreArmor.dst = moduleBase + 0x13186D;
    ignoreArmor.backup();
    secKill.len = 2;
    secKill.dst = moduleBase + 0x13178A;
    secKill.backup();
    secKill.setPatchBytes("\x7C\x1D");

    noCd.len = 2;
    noCd.dst = moduleBase + 0x87296;
    noCd.backup();
    noCd.setPatchBytes("\x7D\x14");

    backRun.len = 1;
    backRun.dst = moduleBase + 0x502C0;
    backRun.backup();
    backRun.setPatchBytes("\xC3");
}


typedef int(__stdcall* _givedamage)(int damage);
typedef void(__stdcall* _picksolar)();

DWORD WINAPI MainThread(HMODULE hModule)
{
    AllocConsole();
    FILE* tmp;
    freopen_s(&tmp, "CONOUT$", "w", stdout);
    helpInfo();
    init();
    _givedamage givedamage = _givedamage(moduleBase + 0x1317c0);//131872 real damage
    _picksolar picksolar = _picksolar(moduleBase + 0x30E40); //
    // 1312FE damage target 
    // 12D718 
    // 131319 DO DAMAGE
    
    while (!GetAsyncKeyState(VK_END)) 
    {
        
        if (GetAsyncKeyState(VK_NUMPAD0) & 1) {
            DWORD solarAddress = mem::FindDMAddress(moduleBase + 0x2A74E8, { 0x68,0x768,0x5560 });
            *(int*)solarAddress += 1000;
            std::cout << "1000solar increased" << std::endl;
        }
        if (GetAsyncKeyState(VK_NUMPAD1) & 1){
            incSolarHook.bHook = !incSolarHook.bHook;
            if (incSolarHook.bHook) {
                incSolarHook.Hook(incSolar);
                std::cout << "incSolar enabled!\n";
            }
            else {
                incSolarHook.restore();
                std::cout << "incSolar disabled!\n";
            }
        }
        if (GetAsyncKeyState(VK_NUMPAD2) & 1){
            bigSolarHook.bHook = !bigSolarHook.bHook;
            if (bigSolarHook.bHook) {
                bigSolarHook.Hook(bigSolar);
                std::cout << "bigSolar enabled!\n";
            }
            else {
                bigSolarHook.restore();
                std::cout << "bigSolar disabled!\n";
            }
        }

        if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
            invinPlantes.bPatch = !invinPlantes.bPatch;
            if (invinPlantes.bPatch) {
                invinPlantes.patch();
                std::cout << "invinPlantes enabled!\n";
            }
            else {
                invinPlantes.restore();
                std::cout << "invinPlantes disabled!\n";
            }
        }
        if (GetAsyncKeyState(VK_NUMPAD4) & 1) {
            secKill.bPatch = !secKill.bPatch;
            if (secKill.bPatch) {
                secKill.patch();
                ignoreArmor.nop();
                std::cout << "secKill enabled!\n";
            }
            else {
                ignoreArmor.restore();
                secKill.restore();
                std::cout << "secKill disabled!\n";
            }
        }
        if (GetAsyncKeyState(VK_NUMPAD5) & 1) {
            noCd.bPatch = !noCd.bPatch;
            if (noCd.bPatch) {
                noCd.patch();
                std::cout << "noCd enabled!\n";
            }
            else {
                noCd.restore();
                std::cout << "noCd disabled!\n";
            }
        }
        if (GetAsyncKeyState(VK_NUMPAD6) & 1) {
            picksolar();
            std::cout << "try to pick\n";
        }
        if (GetAsyncKeyState(VK_HOME) & 1) {
            backRun.bPatch = !backRun.bPatch;
            if (backRun.bPatch) {
                backRun.patch();
                std::cout << "backRun enabled!\n";
            }
            else {
                backRun.restore();
                std::cout << "backRun disabled!\n";
            }

        }
        Sleep(10);
    }
    fclose(tmp);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        HANDLE hackThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0);
        if (hackThread)
        {
            CloseHandle(hackThread);
        }
        break;
    }
    return TRUE;
}


/* incSolarHook incSolar
PlantsVsZombies.exe+1BA74 - 2B F3                 - sub esi,ebx
PlantsVsZombies.exe+1BA76 - 89 B7 60550000        - mov [edi+00005560],esi
*/

/* bigSolarHook bigSolar
 PlantsVsZombies.exe+30A11 - 01 88 60550000        - add [eax+00005560],ecx
*/
/* incSolarHook incSolar
PlantsVsZombies.exe+1BA74 - 2B F3                 - sub esi,ebx
PlantsVsZombies.exe+1BA76 - 89 B7 60550000        - mov [edi+00005560],esi
*/

/* bigSolarHook bigSolar
 PlantsVsZombies.exe+30A11 - 01 88 60550000        - add [eax+00005560],ecx
*/
/*
    13186D cancel armor test
    131783 BODY
    87296 no cd
    313F8 picked solar call
    3158B autopick solar
*/