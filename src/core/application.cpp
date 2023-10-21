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
    ResourceManager::LoadShader("water", "assets/shaders/water.glsl");

    ResourceManager::LoadTexture("swordsman", "assets/textures/units/swordsman.png");
    ResourceManager::LoadTexture("archer", "assets/textures/units/archer.png");
    ResourceManager::LoadTexture("dwarf", "assets/textures/units/dwarf.png");
    ResourceManager::LoadTexture("demon", "assets/textures/units/demon.png");
    ResourceManager::LoadTexture("harpy", "assets/textures/units/harpy.png");

    ResourceManager::LoadTexture("sand", "assets/textures/envs/sand.png");
    ResourceManager::LoadTexture("stone", "assets/textures/envs/stone.png");
    ResourceManager::LoadTexture("tree", "assets/textures/envs/tree.png");

    ResourceManager::LoadTexture("workshop", "assets/textures/buildings/workshop.png");
    ResourceManager::LoadTexture("gold_mine", "assets/textures/buildings/gold_mine.png");
    ResourceManager::LoadTexture("harpy_tower", "assets/textures/buildings/harpy_tower.png");
    ResourceManager::LoadTexture("demon_castle", "assets/textures/buildings/demon_castle.png");
    ResourceManager::LoadTexture("dragon_lair", "assets/textures/buildings/dragon_lair.png");
}

void Application::InitializeColors()
{
    ColorData::Get().TileColors.MiniMapColor = { 0.2f, 0.2f, 0.2f, 1.0f };
    ColorData::Get().TileColors.TileHoverBorderColor = { 0.95f, 0.8f, 0.2f, 1.0f };
    ColorData::Get().UITheme.ShopPanelBackgroundColor = { 0.4f, 0.4f, 0.4f, 1.0f };
    ColorData::Get().UITheme.ShopPanelHighlighUnitGroupColor = { 0.5f, 0.5f, 0.5f, 1.0f };
    ColorData::Get().Resources.Wood = { 0.545f, 0.270f, 0.075f };
    ColorData::Get().Resources.Rock = { 0.5f, 0.5f, 0.5f };
    ColorData::Get().Resources.Steel = { 0.7f, 0.7f, 0.7f };
    ColorData::Get().Resources.Gold = { 0.984f, 0.792f, 0.188f };
}
