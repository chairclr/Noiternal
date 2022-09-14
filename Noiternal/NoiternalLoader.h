#pragma once
#include "framework.h"
#include "LuaExecutor.h"


class NoiternalLoader
{
private:
	static void InternalLoad(HMODULE hMod);
	static void InternalUnload();

	static bool LoadWin32Console();
	static bool UnloadWin32Console();
    static void DebugConsoleThread();

    static bool LoadNoitaLua();
    static bool UnloadNoitaLua();


    static HWND _consoleHandle;
	static HMODULE _noiternalModuleHandle;
    static bool _unloadNoiternal;

public:

	static void Load(HMODULE hMod);
	static void Unload();
};

