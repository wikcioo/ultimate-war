#include "debug_layer.h"

#include "core/application.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

DebugLayer::DebugLayer(const std::string& name)
    : Layer(name)
{
}

void DebugLayer::OnAttach()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    auto window = Application::Get().GetWindow()->GetNativeWindow();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void DebugLayer::OnDetach()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

static void ImGuiBeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

static void ImGuiEndFrame()
{
    auto window = Application::Get().GetWindow();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(window->GetWidth(), window->GetHeight());

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugLayer::OnUpdate(float dt)
{
    ImGuiBeginFrame();

    static bool show_demo_window = true;
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Begin("Info");
    ImGui::Text("FPS: %.2f", 1.0f / dt);
    ImGui::End();

    ImGuiEndFrame();
}

void DebugLayer::OnEvent(Event& event)
{

}
