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

    static HWND _consoleHandle;
    static bool _unloadNoiternal;

public:
	static void Load(HMODULE hMod);
	static void Unload();


    static void SetNoiternalDllDir();

	static HMODULE NoiternalModuleHandle;
    static uintptr_t NoitaModuleAddress;
};

