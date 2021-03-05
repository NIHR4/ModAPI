#include "pch.h"
#include "ModLdrApi.h"
#include <unordered_map>
#include <string>
#include <filesystem>
#include <iostream>

std::unordered_map<ModHandle, MODLDR_MOD_ENTRY> modsList;
using ModlistEntryT = std::pair<ModHandle, MODLDR_MOD_ENTRY>;
ModLdrStatus status;

std::string getDLLName(HMODULE dllModule) {
    char filePath[MAX_PATH];
    GetModuleFileNameA(dllModule, filePath, MAX_PATH);
    return std::filesystem::path(filePath).filename().string();
}

//Public API
ModHandle mod_register_dll(HMODULE dllModule){
    std::string dllName =getDLLName(dllModule);
    const bool nameCollision = std::any_of(
        modsList.cbegin(),
        modsList.cend(),
        [&dllName](const ModlistEntryT& mod) {return dllName == mod.second.modName; });
    
    if (nameCollision) {
        mod_set_error(ML_ALREADY_LOADED_ERR);
        return 0;
    }
    
    MODLDR_MOD_ENTRY entry;
    const size_t strbufSize = dllName.size() + 1;
    char* nameBuff = new char[strbufSize];
    strcpy_s(nameBuff, strbufSize, dllName.data());
    
    entry.displayName = nameBuff;
    entry.modName = nameBuff;
    
    auto handle = reinterpret_cast<ModHandle>(dllModule);
    modsList.insert({ handle, entry });
    mod_set_error(ML_OK);
    return handle;
}

void mod_set_title(ModHandle handle, const char* displayName){
    auto entry = modsList.find(handle);
    if (entry == modsList.end()) {
        mod_set_error(ML_INVALID_HANDLE_ERR);
        return;
    }
    if (entry->second.modName != entry->second.displayName)
        delete[] entry->second.displayName;
    
    const size_t nameSize = strlen(displayName) + 1;
    char* nameBuff = new char[nameSize];
    strcpy_s(nameBuff, nameSize, displayName);
    mod_set_error(ML_OK);
    entry->second.displayName = nameBuff;
}

void mod_add_enable(ModHandle  handle, ModCallback callback) {
    auto entry = modsList.find(handle);
    if (entry == modsList.end()) {
        mod_set_error(ML_INVALID_HANDLE_ERR);
        return;
    }
    mod_set_error(ML_OK);
    entry->second.initializeMod = callback;
}

void mod_add_disable(ModHandle  handle, ModCallback callback) {
    auto entry = modsList.find(handle);
    if (entry == modsList.end()) {
        mod_set_error(ML_INVALID_HANDLE_ERR);
        return;
    }
    mod_set_error(ML_OK);
    entry->second.uninitializeMod = callback;
}

void mod_set_error(ModLdrStatus code) { status = code; }
ModLdrStatus mod_get_error() { return status; }

//Developer API

const MODLDR_MOD_ENTRY* devmod_get_mod_info(ModHandle handle) {
    auto mod = modsList.find(handle);
    if (mod == modsList.end()) return nullptr;
    return &mod->second;
}

ModHandle devmod_fetch_id_with_name(const char* moduleName) {
    auto mod = std::find_if(
        modsList.cbegin(),
        modsList.cend(),
        [moduleName](const ModlistEntryT& entry) {return strcmp(entry.second.modName, moduleName) == 0; });
    
    if (mod == modsList.cend()) return 0;
    return mod->first;
}

void devmod_remove_mod(ModHandle handle){
    auto mod = modsList.find(handle);
    if (mod == modsList.end()) return;
    if (mod->second.modName == mod->second.displayName) {
        delete[] mod->second.modName;
    }
    else {
        delete[] mod->second.modName;
        delete[] mod->second.displayName;
    }
    mod->second.modName = nullptr;
    mod->second.displayName = nullptr;
    modsList.erase(handle);
}



