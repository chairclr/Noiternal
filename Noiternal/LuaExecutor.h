#pragma once
#include "framework.h"
#include <lua.hpp>
#include <MinHook.h>
#include "LuaManager.h"
#include "std_string.h"
#include "NoiternalLoader.h"

class LuaExecutor
{
public:
    static LuaManager* GameState;

    static void HookLua();
    static void UnhookLua();

private:

};