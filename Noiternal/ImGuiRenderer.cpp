#include "ImGuiRenderer.h"

bool ImGuiRenderer::BackendInit = false;
bool ImGuiRenderer::ContextInit = false;

void* oSDL_GL_SwapWindow;
void* oSDL_PollEventHook;
void ImGuiRenderer::HookRendering()
{
    void* swapWindowPtr = (void*)(GetProcAddress(NoiternalLoader::SDL2ModuleHandle, "SDL_GL_SwapWindow"));
    void* pollEventPtr = (void*)(GetProcAddress(NoiternalLoader::SDL2ModuleHandle, "SDL_PollEvent"));

    MH_CreateHook(swapWindowPtr, ImGuiRenderer::SDL_GL_SwapWindowHook, &oSDL_GL_SwapWindow);
    MH_EnableHook(swapWindowPtr);

    MH_CreateHook(pollEventPtr, ImGuiRenderer::SDL_PollEventHook, &oSDL_PollEventHook);
    MH_EnableHook(pollEventPtr);
}

void ImGuiRenderer::UnhookRendering()
{
    void* swapWindowPtr = (void*)(GetProcAddress(NoiternalLoader::SDL2ModuleHandle, "SDL_GL_SwapWindow"));
    void* pollEventPtr = (void*)(GetProcAddress(NoiternalLoader::SDL2ModuleHandle, "SDL_PollEvent"));

    MH_DisableHook(swapWindowPtr);
    MH_RemoveHook(swapWindowPtr);

    MH_DisableHook(pollEventPtr);
    MH_RemoveHook(pollEventPtr);
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

void ImGuiRenderer::SDL_GL_SwapWindowHook(SDL_Window* window)
{
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
    reinterpret_cast<void(*)(SDL_Window* window)>(oSDL_GL_SwapWindow)(window);
}

int ImGuiRenderer::SDL_PollEventHook(SDL_Event* event)
{
    int result = reinterpret_cast<int(*)(SDL_Event * event)>(oSDL_PollEventHook)(event);

    if (BackendInit && ContextInit)
    {
        if (event && result)
        {
            ImGui_ImplSDL2_ProcessEvent(event);

            ImGuiIO& io = ImGui::GetIO();

            switch (event->type)
            {
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEWHEEL:
            {
                if (io.WantCaptureMouse || io.WantTextInput)
                {
                    SDL_PollEventHook(event);
                    // prevent noita from processing event imo (in my opinion)
                    result = 0;
                    *event = {};
                }
                break;
            }

            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                if (io.WantTextInput)
                {
                    SDL_PollEventHook(event);
                    // prevent noita from processing event imo (in my opinion)
                    result = 0;
                    *event = {};
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

    ImGui::Begin("Test");

    if (ImGui::Button("Run"))
    {
        if (LuaExecutor::GameState == nullptr || LuaExecutor::GameState->LuaState == nullptr)
        {
            std::cout << "Invalid Lua State\n";
        }
        else
        {
            luaL_dostring(LuaExecutor::GameState->LuaState, workingLuaText.c_str());
        }
    }
    ImGui::InputTextMultiline("##Lua", &workingLuaText);

    ImGui::End();
}

void ImGuiRenderer::PostDraw()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}