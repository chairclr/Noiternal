#include "NoiternalLoader.h"

HWND NoiternalLoader::_consoleHandle = 0;
HMODULE NoiternalLoader::NoiternalModuleHandle = 0;
bool NoiternalLoader::_unloadNoiternal = 0;
uintptr_t NoiternalLoader::NoitaModuleAddress = NULL;

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
    NoitaModuleAddress = (uintptr_t)GetModuleHandleA("noita.exe");
	NoiternalModuleHandle = hMod;

    

    SetNoiternalDllDir();
	LoadWin32Console();
    MH_Initialize();
    LuaExecutor::HookLua();

    HANDLE debugConsoleThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)DebugConsoleThread, nullptr, 0, nullptr);

    // need to fix this being ugly :sad:
    while (true)
    {
        if (_unloadNoiternal) break;
        Sleep(16); // ~60 fps
    }

    if (debugConsoleThread)
    {
        WaitForSingleObject(debugConsoleThread, 1000);
    }
    InternalUnload();
}
void NoiternalLoader::InternalUnload()
{
	UnloadWin32Console();
    LuaExecutor::UnhookLua();
    FreeLibraryAndExitThread(NoiternalModuleHandle, 0);
}

void NoiternalLoader::SetNoiternalDllDir()
{
    char noiternalPath[MAX_PATH];
    char noiternalDir[MAX_PATH];
    GetModuleFileName(NoiternalModuleHandle, noiternalPath, sizeof(noiternalPath));
    _splitpath_s(noiternalPath, nullptr, 0, noiternalDir, MAX_PATH, nullptr, 0, nullptr, 0);
    SetDllDirectory(noiternalDir);
}

bool NoiternalLoader::LoadWin32Console()
{
	if (!AllocConsole()) return false;

    FILE* file;
	if (freopen_s(&file, "conin$", "r", stdin)) return false;
	if (freopen_s(&file, "conout$", "w", stdout)) return false;

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
            if (LuaExecutor::GameState == nullptr || LuaExecutor::GameState->LuaState == nullptr)
            {
                std::cout << "Invalid Lua State\n";
                continue;
            }
        
            std::string str = s.substr(5);
        
            luaL_dostring(LuaExecutor::GameState->LuaState, str.c_str());
        }
        else
        {
            std::cout << s << "\n";
        }
    }
}