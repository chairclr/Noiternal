#include "ImGuiRenderer.h"

bool ImGuiRenderer::BackendInit = false;
bool ImGuiRenderer::ContextInit = false;

Hook SDL_GL_SwapWindowHook;
Hook SDL_PollEventHook;
void ImGuiRenderer::HookRendering()
{
    SDL_GL_SwapWindowHook = Hook(NoiternalLoader::SDL2ModuleHandle, "SDL_GL_SwapWindow", &SDL_GL_SwapWindowHookFunc);
    SDL_GL_SwapWindowHook.Enable();

    SDL_PollEventHook = Hook(NoiternalLoader::SDL2ModuleHandle, "SDL_PollEvent", &SDL_PollEventHookFunc);
    SDL_PollEventHook.Enable();
}

void ImGuiRenderer::UnhookRendering()
{
    SDL_GL_SwapWindowHook.Remove();
    SDL_PollEventHook.Remove();
}

void ImGuiRenderer::CreateContext()
{
    if (ContextInit) throw std::runtime_error("ImGuiRenderer context already created"); // Sanity check

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    SetStyle();
    // Add Noita font later on
    // io.Fonts->AddFontFromFileTTF("NoitaPixel.ttf", 20);
    io.Fonts->AddFontDefault();

    ContextInit = true;
}
void ImGuiRenderer::CreateBackend(SDL_Window* window, SDL_GLContext glContext)
{
    if (BackendInit) throw std::runtime_error("ImGuiRenderer backend already created"); // Sanity check
    if (!ContextInit) throw std::runtime_error("ImGuiRenderer context not created"); // Sanity check

    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    if (!ImGui_ImplOpenGL3_Init("#version 110"))
    {
        throw std::runtime_error("ImGuiRenderer failed to init OpgenGL3");
    }

    BackendInit = true;
}

void ImGuiRenderer::SetStyle()
{
    // Implement default style later
}

void ImGuiRenderer::SDL_GL_SwapWindowHookFunc(SDL_Window* window)
{
    auto oSDL_GL_SwapWindow = HFUNC(void, (SDL_Window* window), SDL_GL_SwapWindowHook.OriginalFunction);

    if (!BackendInit)
    {
        CreateBackend(SDL_GL_GetCurrentWindow(), SDL_GL_GetCurrentContext());
    }
    if (ContextInit)
    {
        PreDraw();
        Draw();
        PostDraw();
    }
    oSDL_GL_SwapWindow(window);
}

int ImGuiRenderer::SDL_PollEventHookFunc(SDL_Event* event)
{
    
    auto oSDL_PollEvent = HFUNC(int, (SDL_Event* event), SDL_PollEventHook.OriginalFunction);

    int result = oSDL_PollEvent(event);

    if (ContextInit && BackendInit)
    {
        if (event && result)
        {
            ImGuiIO& io = ImGui::GetIO();
            ImGui_ImplSDL2_ProcessEvent(event);

            switch (event->type)
            {
            case SDL_MOUSEMOTION:
            case SDL_MOUSEWHEEL:
            case SDL_MOUSEBUTTONDOWN:
            {
                if (io.WantCaptureMouse || io.WantTextInput)
                {
                    result = SDL_PollEventHookFunc(event);
                }
                break;
            }

            case SDL_KEYDOWN:
            {
                if (io.WantTextInput)
                {
                    result = SDL_PollEventHookFunc(event);
                }
                break;
            }
            default:
                break;
            }
        }
    }

    return result;
}

void ImGuiRenderer::PreDraw()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

std::string workingLuaText = "";
void ImGuiRenderer::Draw()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Begin("Test", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Misc."))
        {
            if (ImGui::MenuItem("Unload"))
            {
                NoiternalLoader::Unload();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("Lua State: %s", (LuaExecutor::GameState == nullptr || LuaExecutor::GameState->LuaState == nullptr) ? "invalid" : "valid");

    if (ImGui::Button("Run"))
    {
        if (LuaExecutor::GameState != nullptr && LuaExecutor::GameState->LuaState != nullptr)
        {
            luaL_dostring(LuaExecutor::GameState->LuaState, workingLuaText.c_str());
        }
    }
    ImGui::InputTextMultiline("##Lua", &workingLuaText, ImGui::GetContentRegionAvail());

    ImGui::End();

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
}

void ImGuiRenderer::PostDraw()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}