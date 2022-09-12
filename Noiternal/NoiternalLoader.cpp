#include "NoiternalLoader.h"

#pragma warning(disable : 4996)

HWND NoiternalLoader::_consoleHandle = 0;
HMODULE NoiternalLoader::_noiternalModuleHandle = 0;
bool NoiternalLoader::_unloadNoiternal = 0;

void NoiternalLoader::Load(HMODULE hMod)
{
	static bool _loaded = false;
	if (_loaded) return;
	_loaded = true;

    CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InternalLoad, hMod, 0, nullptr);
}
void NoiternalLoader::Unload()
{
    _unloadNoiternal = true;
}

void NoiternalLoader::InternalLoad(HMODULE hMod)
{
	_noiternalModuleHandle = hMod;
	LoadWin32Console();

    // TODO: Use Noita's lua51.dll to execute arbitrary lua code without mods 

    HANDLE h = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)DebugConsoleThread, nullptr, 0, nullptr);

    // need to fix this being ugly :sad:
    while (true)
    {
        if (_unloadNoiternal) break;
        Sleep(16); // ~60 fps
    }

    if (h) WaitForSingleObject(h, INFINITE);
    InternalUnload();
}
void NoiternalLoader::InternalUnload()
{
	UnloadWin32Console();
    FreeLibraryAndExitThread(_noiternalModuleHandle, 0);
}

bool NoiternalLoader::LoadWin32Console()
{
	if (!AllocConsole()) return false; // hopefully AllocConsole does not return nullptr when theres an already existing console window
	if (!std::freopen("conin$", "r", stdin)) return false;
	if (!std::freopen("conout$", "w", stdout)) return false;
	if (!std::freopen("conout$", "w", stderr)) return false;

	_consoleHandle = GetConsoleWindow();
	return true;
}
bool NoiternalLoader::UnloadWin32Console()
{
    if (!_consoleHandle) return false;
	FreeConsole();
	PostMessage(_consoleHandle, WM_CLOSE, 0, 0);
    return true;
}
void NoiternalLoader::DebugConsoleThread()
{
    // simple console just as a demo, not important
    while (true)
    {
        if (_unloadNoiternal) break;

        std::string s;

        std::cin >> s;

        if (s == "unload")
        {
            Unload();
        }
        else if (s == "exit")
        {
            exit(0);
        }
        else
        {
            std::cout << s << "\n";
        }
    }
}