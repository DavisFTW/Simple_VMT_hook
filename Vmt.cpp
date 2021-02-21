// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>

using functionT = void(__thiscall*)(std::uintptr_t* thisptr);
functionT original_function;

void __fastcall hooked_function(std::uintptr_t* pointer, int edx)
{
    std::cout << "hi from dll" << std::endl;

    // call the original function
    original_function(pointer);
}

DWORD WINAPI entry(HMODULE hModule)
{
    DWORD virtual_address   = 0x317C; // RVA is the last 4 digits of address
    DWORD old_protection    = 0 ;
    DWORD base_address      = 0;
    DWORD RVA               = 0;
    // get base address

    base_address = (DWORD)GetModuleHandleA(nullptr);


    // get vmt from RvA
    std::uintptr_t* vTable = reinterpret_cast<std::uintptr_t*>((DWORD)GetModuleHandleA(nullptr) + virtual_address);   // get our vTable
   
    VirtualProtect(vTable, 4, PAGE_READWRITE, &old_protection); // remove the protection

    // store the original function

    original_function = reinterpret_cast<functionT>(*vTable);

    // swap our pointers

    *vTable = reinterpret_cast<std::uintptr_t>(&hooked_function);

    // restore the old protection

    VirtualProtect(vTable, 4, old_protection, 0);

    while (true)
    {
        // do not use GetAsyncKeyState() in internals, there is a better way
        if (GetAsyncKeyState(VK_F1))  
        {
            //unhook our hooked feature
            *vTable = reinterpret_cast<std::uintptr_t>(original_function);  
        }
            
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {   
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);
        auto handle = CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)entry(hModule), hModule, NULL, nullptr);
        if (handle)
            CloseHandle(handle);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

