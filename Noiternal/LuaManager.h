#pragma once
#include <lua.hpp>

struct LuaManager
{
    void* VirtualFunctionTable;
    char padd1[52];
    lua_State* LuaState;
};