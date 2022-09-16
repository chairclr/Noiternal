#pragma once
#include "framework.h"
#include <MinHook.h>

class Hook
{
public:
    Hook();
    Hook(void* from, void* to);
    Hook(HMODULE fromModule, const char* exportName, void* to);
    Hook(HMODULE fromModule, std::string& exportName, void* to);

    /// <summary>
    /// Enables a hook
    /// </summary>
    bool Enable();

    /// <summary>
    /// Disables a hook
    /// </summary>
    bool Disable();


    /// <summary>
    /// (Automatically called when constructor succeeds) Actually calls MH_CreateHook
    /// </summary>
    bool Create();

    /// <summary>
    /// Actually calls MH_RemoveHook
    /// </summary>
    bool Remove();

    void* From;
    void* To;
    void* OriginalFunction;


private:

    bool _created;
    bool _enabled;

    void InitHookInternal(void* from, void* to);
};