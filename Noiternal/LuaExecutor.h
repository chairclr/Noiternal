#pragma once
#include "framework.h"
#include <lua.hpp>
#include "LuaManager.h"
#include "std_string.h"
#include <MinHook.h>
#include "NoiternalLoader.h"

class LuaExecutor
{
public:
    static LuaManager* GameState;

    static void HookLua();
    static void UnhookLua();

private:

};