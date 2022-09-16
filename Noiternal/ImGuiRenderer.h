#pragma once
#include "framework.h"
#include "NoiternalLoader.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include "Hook.h"


class ImGuiRenderer
{
public:
    static void CreateContext();
    static void HookRendering();
    static void UnhookRendering();

    static void SetStyle();

    static bool ContextInit;
    static bool BackendInit;

private:

    static void CreateBackend(SDL_Window* window, SDL_GLContext gl_context);

    static void PreDraw();
    static void Draw();
    static void PostDraw();

    static void SDL_GL_SwapWindowHookFunc(SDL_Window* window);
    static int SDL_PollEventHookFunc(SDL_Event* event);
};