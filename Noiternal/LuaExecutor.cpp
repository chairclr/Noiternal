#include "LuaExecutor.h"

// noita.exe+0x31CDD0
#define NOITA_NEW_LUA 0x31CDD0


lua_State* LuaExecutor::GameState = nullptr;
uintptr_t LuaExecutor::_noitaModuleAddress = NULL;

void* oLuaNewState = nullptr;
void __fastcall SetNewLuaStateHook(uintptr_t* pThis)
{
    reinterpret_cast<void(*)(uintptr_t * param)>(oLuaNewState)(pThis);

    LuaExecutor::GameState = (lua_State*)(pThis[0xe]);
}

void LuaExecutor::HookLua()
{
    _noitaModuleAddress = (uintptr_t)GetModuleHandleA("noita.exe");

    MH_STATUS status = MH_CreateHook((void*)(_noitaModuleAddress + NOITA_NEW_LUA), SetNewLuaStateHook, &oLuaNewState);
    MH_STATUS status2 = MH_EnableHook((void*)(_noitaModuleAddress + NOITA_NEW_LUA));
}
void LuaExecutor::UnhookLua()
{
    MH_STATUS status = MH_DisableHook((void*)(_noitaModuleAddress + NOITA_NEW_LUA));
    MH_STATUS status2 = MH_RemoveHook((void*)(_noitaModuleAddress + NOITA_NEW_LUA));
}