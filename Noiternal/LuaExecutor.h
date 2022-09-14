#pragma once
#include "framework.h"
#include <lua.hpp>
#include <MinHook.h>


class LuaExecutor
{
public:
    static lua_State* GameState;

    static void HookLua();
    static void UnhookLua();

private:
    static uintptr_t _noitaModuleAddress;

};