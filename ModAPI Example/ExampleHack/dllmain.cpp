// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "ModLdrApi.h"
#pragma comment(lib,"ModLdrApi.lib")
void __stdcall enableCallback() {
    MessageBoxA(0, "I've been  enabled!", "Mod", MB_OK);
}

void __stdcall disableCallback() {
    MessageBoxA(0, "I've been disabled!", "Mod", MB_OK);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        ModHandle h = mod_register_dll(hModule);
        if (h) {
            mod_add_enable(h, enableCallback);
            mod_add_disable(h, disableCallback);
            mod_set_title(h, "Awesome Mod");
        }
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

