// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "ModLdrApi.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <conio.h>

#pragma comment(lib,"ModLdrApi.lib")
namespace fs = std::filesystem;
void printModData(ModHandle h) {
    const MODLDR_MOD_ENTRY* data = devmod_get_mod_info(h);
    if (data == nullptr) {
        std::cout << "Mod has not been registered using ModAPI\n\n";
        return;
    }
    std::cout << "Mod Display Name: " << data->displayName << "\n";
    std::cout << "Mod DLL Name: " << data->modName << "\n";
    std::cout << "Mod Enable Function: " << std::hex << data->initializeMod << "\n";
    std::cout << "Mod Disable Function: " << std::hex << data->uninitializeMod << "\n\n";
}

DWORD WINAPI ldrThread(void* hmod) {
    AllocConsole();
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
    LoadLibraryA("ModLdrApi.dll");
    for (;;) {
        std::string dllname;
        std::cout << "Name of the DLL you want to load:";
        std::getline(std::cin, dllname);
        if (!fs::exists(dllname)) {
            std::cout << "File doesn't exist\n\n";
            continue;
        }
        HMODULE dllMod = LoadLibraryA(dllname.c_str());
        if (dllMod == NULL) {
            std::cout << "LoadLibrary error\n\n";
            continue;
        }

        //You could avoid calling this function and instead just use LoadLibrary's return value
        //I'm using fetch_id for demonstration purposes
        ModHandle h = devmod_fetch_id_with_name(dllname.c_str());
        printModData(h);
        if (h) {
            auto data = devmod_get_mod_info(h);
            std::cout << "Executing mod's Enable Funtion\n";
            data->initializeMod();
            std::cout << "Press ANY key to disable the mod\n";
            _getch();
            data->uninitializeMod();
            devmod_remove_mod(h);
            FreeLibrary(dllMod);
        }
        std::cout << "\n\n";

    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, ldrThread, hModule, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

