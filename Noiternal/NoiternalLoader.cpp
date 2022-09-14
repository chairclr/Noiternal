#include "NoiternalLoader.h"

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

    char noiternalPath[MAX_PATH];
    char noiternalDir[MAX_PATH];
    GetModuleFileName(_noiternalModuleHandle, noiternalPath, sizeof(noiternalPath));
    _splitpath_s(noiternalPath, nullptr, 0, noiternalDir, MAX_PATH, nullptr, 0, nullptr, 0);
    SetDllDirectory(noiternalDir);
	LoadWin32Console();
    MH_Initialize();
    LoadNoitaLua();

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
    UnloadNoitaLua();
    FreeLibraryAndExitThread(_noiternalModuleHandle, 0);
}

bool NoiternalLoader::LoadWin32Console()
{
	if (!AllocConsole()) return false;

    FILE* file;
	if (freopen_s(&file, "conin$", "r", stdin)) return false;
	if (freopen_s(&file, "conout$", "w", stdout)) return false;
	if (freopen_s(&file, "conout$", "w", stderr)) return false;

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
        
        std::getline(std::cin, s);
        
        if (s == "unload")
        {
            Unload();
        }
        else if (s.substr(0, 5) == "eval ")
        {
            if (LuaExecutor::GameState == nullptr)
            {
                std::cout << "Must be injected from the title screen to execute lua\n";
                continue;
            }

            // remove the eval at the start
            std::string str = s.substr(5);

            luaL_dostring(LuaExecutor::GameState, str.c_str());
        }
        else
        {
            std::cout << s << "\n";
        }
    }
}

bool NoiternalLoader::LoadNoitaLua()
{
    LuaExecutor::HookLua();
    return true;
}
bool NoiternalLoader::UnloadNoitaLua()
{
    LuaExecutor::UnhookLua();
    return true;
}