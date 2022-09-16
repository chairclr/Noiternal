#include "Hook.h"

Hook::Hook()
{
    From = nullptr;
    To = nullptr;
    OriginalFunction = nullptr;
    _created = false;
    _enabled = false;
}
Hook::Hook(void* from, void* to)
{
    InitHookInternal(from, to);
}
Hook::Hook(HMODULE fromModule, const char* exportName, void* to)
{
    void* from = (void*)(GetProcAddress(fromModule, exportName));
    InitHookInternal(from, to);
}
Hook::Hook(HMODULE fromModule, std::string& exportName, void* to)
{
    void* from = (void*)(GetProcAddress(fromModule, exportName.c_str()));
    InitHookInternal(from, to);
}

void Hook::InitHookInternal(void* from, void* to)
{
    From = from;
    To = to;
    OriginalFunction = nullptr;
    _created = false;
    _enabled = false;

    if (From == nullptr || To == nullptr) throw std::runtime_error("Invalid target or detour");

    Create();
}

bool Hook::Create()
{
    if (_created) return false;
     _created = MH_CreateHook(From, To, &OriginalFunction) == MH_OK;
    return _created;
}
bool Hook::Remove()
{
    if (!_created) return false;
    if (_enabled) Disable();
    _created = !(MH_RemoveHook(From) == MH_OK);
    return _created;
}

bool Hook::Enable()
{
    if (!_created || _enabled) return false;
    _enabled = MH_EnableHook(From) == MH_OK;
    return _enabled;
}
bool Hook::Disable()
{
    if (!_created || !_enabled) return false;
    _enabled = !(MH_DisableHook(From) == MH_OK);
    return _enabled;
}