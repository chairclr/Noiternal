#include "LuaExecutor.h"

#define NOITA_NEW_LUA               0x0031CDD0
#define NOITA_NEEDS_RESET_LUA_VAR   0x00BF10AA

LuaManager* LuaExecutor::GameState = nullptr;

void* oLuaManagerNewState = nullptr;
void __fastcall LuaManagerNewStateHook(LuaManager* pThis)
{
    reinterpret_cast<void(*)(LuaManager* pThis)>(oLuaManagerNewState)(pThis);
    LuaExecutor::GameState = pThis;
}

void ResetGameLua()
{
    // sus af
    *(uint8_t*)(NoiternalLoader::NoitaModuleAddress + NOITA_NEEDS_RESET_LUA_VAR) = 0;
}

void LuaExecutor::HookLua()
{
    MH_CreateHook((void*)(NoiternalLoader::NoitaModuleAddress + NOITA_NEW_LUA), LuaManagerNewStateHook, &oLuaManagerNewState);
    MH_EnableHook((void*)(NoiternalLoader::NoitaModuleAddress + NOITA_NEW_LUA));
    
    ResetGameLua();
}
void LuaExecutor::UnhookLua()
{
    MH_DisableHook((void*)(NoiternalLoader::NoitaModuleAddress + NOITA_NEW_LUA));
    MH_RemoveHook((void*)(NoiternalLoader::NoitaModuleAddress + NOITA_NEW_LUA));
}