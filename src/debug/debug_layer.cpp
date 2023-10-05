#if defined(DEBUG)

#include "debug_layer.h"

#include <array>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "debug/debug_data.h"
#include "core/application.h"
#include "core/resource_manager.h"

DebugLayer::DebugLayer()
    : Layer("DebugLayer"), m_GameLayer(GameLayer::Get())
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

void DebugLayer::OnEvent(Event& event)
{
    static auto& io = ImGui::GetIO();

    if (event.GetCategory() == EventCategory::Mouse)
        event.Handled |= io.WantCaptureMouse;
    else if (event.GetCategory() == EventCategory::Key)
        event.Handled |= io.WantCaptureKeyboard;
}

void DebugLayer::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void DebugLayer::EndFrame()
{
    auto window = Application::Get().GetWindow();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(window->GetWidth(), window->GetHeight());

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static float AverageDt(float dt)
{
    static int index = 0;
    static float sum = 0.0f;
    static std::array<float, 30> buffer = {};

    sum -= buffer[index];
    sum += dt;
    buffer[index++] = dt;

    if (index == 30)
        index = 0;

    return sum / buffer.size();
}

void DebugLayer::DisplayInfoWindow(float dt)
{
    static bool show_info_window = true;
    if (!show_info_window) return;

    ImGui::Begin("Info", &show_info_window);

    ImGui::Text("Frame rate: %.2f", glm::round(1 / AverageDt(dt)));

    ImGui::Separator();

    auto camera = m_GameLayer.m_CameraController->GetCamera();
    auto pos = camera->GetPosition();
    ImGui::Text("Camera");
    ImGui::Text("  position: %.4f, %.4f, %.4f", pos.x, pos.y, pos.z);
    ImGui::Text("  aspect ratio: %.4f", camera->GetAspectRatio());

    ImGui::Separator();

    auto window = Application::Get().GetWindow();
    ImGui::Text("Window");
    ImGui::Text("  width: %dpx", window->GetWidth());
    ImGui::Text("  height: %dpx", window->GetHeight());
    ImGui::Text("  vsync: %s", window->IsVSyncEnabled() ? "enabled" : "disabled");

    ImGui::End();
}

void DebugLayer::DisplaySettingsWindow()
{
    static bool show_settings_window = true;
    if (!show_settings_window) return;

    ImGui::Begin("Settings", &show_settings_window);

    static bool show_demo_window = false;
    ImGui::Checkbox("Show demo window", &show_demo_window);
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    static bool polygon_mode = false;
    static bool polygon_mode_active_last_frame = false;
    ImGui::Checkbox("Polygon mode", &polygon_mode);
    if (polygon_mode != polygon_mode_active_last_frame)
    {
        if (polygon_mode)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            polygon_mode_active_last_frame = true;
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            polygon_mode_active_last_frame = false;
        }
    }

    if (ImGui::Button("Select map.."))
        ImGui::OpenPopup("available_maps_popup");

    if (ImGui::BeginPopup("available_maps_popup"))
    {
        for (std::string mapName : m_GameLayer.m_GameMapManager->GetAvailableMaps())
            if (ImGui::Selectable(mapName.c_str()))
                m_GameLayer.m_GameMapManager->Load(mapName);
        ImGui::EndPopup();
    }

    ImGui::SameLine();
    std::string selectedMap = m_GameLayer.m_GameMapManager->GetSelectedMapName();
    ImGui::Text("%s", std::string("Selected map: " + (selectedMap.empty() ? "None" : selectedMap)).c_str());

    ImGui::Separator();

    ImGui::Text("Unit settings");
    ImGui::SliderInt("Unit rows", &Tile::s_UnitRows, 1, 3);
    ImGui::SliderInt("Units per row", &Tile::s_UnitsPerRow, 3, 7);
    ImGui::SliderInt("Unit width/offset ratio", &Tile::s_UnitWidthToOffsetRatio, 1, 19);

    ImGui::Separator();

    ImGui::Text("Tile settings");
    ImGui::SliderFloat("Height ratio", &Tile::s_BackgroundHeightRatio, 0.1f, 1.0f);

    ImGui::Separator();

    ImGui::Text("Minimap settings");
    ImGui::SliderFloat("Border", &DebugData::Get()->MinimapData.BorderThickness, 0.01f, 0.1f);
    ImGui::SliderFloat("Zoom", &DebugData::Get()->MinimapData.Zoom, 1.0f, 10.0f);

    ImGui::End();
}

void DebugLayer::DisplayFontSettingsWindow()
{
    static bool show_font_settings_window = true;
    if (!show_font_settings_window) return;

    ImGui::Begin("Font settings", &show_font_settings_window);

    static std::vector<std::string> availableFontNames = ResourceManager::GetAvailableFontNames();

    if (ImGui::Button("Select font.."))
        ImGui::OpenPopup("available_fonts_popup");

    if (ImGui::BeginPopup("available_fonts_popup"))
    {
        for (const auto& fontName : availableFontNames)
            if (ImGui::Selectable(fontName.c_str()))
                DebugData::Get()->Font.FontName = fontName;
        ImGui::EndPopup();
    }

    ImGui::SameLine();
    std::string selectedFontName = DebugData::Get()->Font.FontName;
    ImGui::Text("%s", std::string("Selected font: " + (selectedFontName.empty() ? "None" : selectedFontName)).c_str());

    ImGui::ColorEdit3("Color", &DebugData::Get()->Font.Color[0]);

    ImGui::InputTextMultiline("##label", DebugData::Get()->Font.Text, 100, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4));
    ImGui::SliderFloat("Scale", &DebugData::Get()->Font.Scale, 0.2f, 5.0f);
    ImGui::SliderFloat("PosX", &DebugData::Get()->Font.Pos.x, -2.0f, 2.0f);
    ImGui::SliderFloat("PosY", &DebugData::Get()->Font.Pos.y, -1.0f, 1.0f);

    static int font_alignment = 1;
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Alignment");
    ImGui::SameLine();
    ImGui::RadioButton("LEFT", &font_alignment, 0);
    ImGui::SameLine();
    ImGui::RadioButton("MIDDLE", &font_alignment, 1);
    ImGui::SameLine();
    ImGui::RadioButton("RIGHT", &font_alignment, 2);
    if (font_alignment == 0)
        DebugData::Get()->Font.Alignment = TextAlignment::LEFT;
    else if (font_alignment == 1)
        DebugData::Get()->Font.Alignment = TextAlignment::MIDDLE;
    else if (font_alignment == 2)
        DebugData::Get()->Font.Alignment = TextAlignment::RIGHT;

    ImGui::End();
}

void DebugLayer::DisplayPlayerWindow()
{
    ImGui::Begin("Players");

    ImGui::Text("Iteration nr: %d", m_GameLayer.m_IterationNumber);

    auto currentPlayer = m_GameLayer.GetPlayerManager()->GetCurrentPlayer();
    ImGui::Text("Current player: %s", currentPlayer->GetName().c_str());

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Players"))
    {
        for (auto player : m_GameLayer.GetPlayerManager()->GetAllPlayers())
        {
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode(player->GetName().c_str()))
            {
                ImGui::Text("gold: (%d)", player->GetGold());
                ImGui::Text("owned tiles: (%zu)", player->GetOwnedTiles().size());
                auto clr = player->GetColor();
                ImGui::Text("color: "); ImGui::SameLine();
                ImGui::ColorButton("player color", ImVec4(clr.r, clr.g, clr.b, 1.0f));
                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    ImGui::End();
}

void DebugLayer::OnUpdate(float dt)
{
    BeginFrame();

    DisplayInfoWindow(dt);
    DisplaySettingsWindow();
    DisplayFontSettingsWindow();
    DisplayPlayerWindow();

    EndFrame();
}

#endif
