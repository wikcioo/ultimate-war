#include "application.h"

#include "game/game_layer.h"
#include "core/core.h"
#include "core/logger.h"
#include "core/resource_manager.h"
#include "debug/debug_layer.h"
#include "graphics/renderer.h"
#include "ui/ui_layer.h"

Application* Application::s_Instance = nullptr;

Application::Application()
{
    s_Instance = this;

    Logger::Init();
    m_Window = std::make_unique<Window>();
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

    LoadResources();
    InitializeColors();

    Renderer2D::Init();

    GameLayer* gameLayer = new GameLayer();
    gameLayer->OnAttach();
    m_LayerStack.PushLayer(gameLayer);

    UILayer* uiLayer = new UILayer();
    uiLayer->OnAttach();
    m_LayerStack.PushOverlay(uiLayer);

#if defined(DEBUG)
    auto debugLayer = new DebugLayer();
    debugLayer->OnAttach();
    m_LayerStack.PushOverlay(debugLayer);
#endif
}

Application::~Application()
{
}

void Application::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowClosedEvent>(BIND_EVENT_FN(Application::OnWindowClose));

    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
    {
        if (event.Handled)
            break;
        (*it)->OnEvent(event);
    }
}

bool Application::OnWindowClose(WindowClosedEvent& event)
{
    m_Running = false;
    return true;
}

void Application::Run()
{
    static float lastTime = 0.0f;
    while (m_Running)
    {
        float now = (float)glfwGetTime();
        m_DeltaTime = now - lastTime;
        lastTime = now;

        for (auto layer : m_LayerStack)
            layer->OnUpdate(m_DeltaTime);

        m_Window->OnUpdate();
    }
}

void Application::LoadResources()
{
    ResourceManager::LoadFont("vinque", "assets/fonts/vinque/vinque.otf");
    ResourceManager::LoadFont("rexlia", "assets/fonts/rexlia/rexlia.otf");

    ResourceManager::LoadShader("font", "assets/shaders/font.glsl");
    ResourceManager::LoadShader("color", "assets/shaders/color.glsl");
    ResourceManager::LoadShader("texture", "assets/shaders/texture.glsl");

    ResourceManager::LoadTexture("swordsman", "assets/textures/units/swordsman.png");
    ResourceManager::LoadTexture("archer", "assets/textures/units/archer.png");
    ResourceManager::LoadTexture("dwarf", "assets/textures/units/dwarf.png");
    ResourceManager::LoadTexture("demon", "assets/textures/units/demon.png");
    ResourceManager::LoadTexture("harpy", "assets/textures/units/harpy.png");
}

void Application::InitializeColors()
{
    ColorData::Get().TileColors.MiniMapColor = {0.2f, 0.2f, 0.2f, 1.0f};
    ColorData::Get().TileColors.TileHoverBorderColor = {0.2f, 0.3f, 0.8f, 1.0f};
    ColorData::Get().UITheme.UnitPanelBackgroundColor = {0.2f, 0.2f, 0.2f, 1.0f};
    ColorData::Get().UITheme.UnitPanelHighlighUnitColor = {0.5f, 0.5f, 0.5f, 1.0f};
}
