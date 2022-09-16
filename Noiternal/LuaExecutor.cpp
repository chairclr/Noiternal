#include "LuaExecutor.h"

#define NOITA_NEW_LUA               0x0031CDD0
#define NOITA_NEEDS_RESET_LUA_VAR   0x00BF10AA

LuaManager* LuaExecutor::GameState = nullptr;

void ResetGameLua()
{
    // sus af
    *(uint8_t*)(NoiternalLoader::NoitaModuleAddress + NOITA_NEEDS_RESET_LUA_VAR) = 0;
}

Hook LuaManagerNewStateHook;
void __fastcall LuaManagerNewStateHookFunc(LuaManager* pThis)
{
    reinterpret_cast<void(*)(LuaManager* pThis)>(LuaManagerNewStateHook.OriginalFunction)(pThis);
    LuaExecutor::GameState = pThis;
}

void LuaExecutor::HookLua()
{
    LuaManagerNewStateHook = Hook((void*)(NoiternalLoader::NoitaModuleAddress + NOITA_NEW_LUA), LuaManagerNewStateHookFunc);
    LuaManagerNewStateHook.Enable();
    
    ResetGameLua();
}
void LuaExecutor::UnhookLua()
{
    LuaManagerNewStateHook.Remove();
}