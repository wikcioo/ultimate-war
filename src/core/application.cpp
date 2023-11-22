#include "application.h"

#include "core/resource_manager.h"
#include "graphics/renderer.h"
#include "loader/save_loader.h"
#include "loader/save_loader_exception.h"

Application* Application::s_Instance = nullptr;

Application::Application()
{
    s_Instance = this;

    Logger::Init();
    m_Window = std::make_unique<Window>();
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
    m_LayerStack = std::make_unique<LayerStack>();

    LoadResources();
    InitializeColors();

    Renderer2D::Init();

    m_MainMenuLayer = std::make_shared<MainMenuLayer>();
    m_MainMenuLayer->OnAttach();
    m_LayerStack->PushLayer(m_MainMenuLayer);

#if defined(DEBUG)
    DebugLayer::InitImGui();
#endif
}

Application::~Application()
{
    Renderer2D::Shutdown();

#if defined(DEBUG)
    DebugLayer::ShutdownImGui();
#endif
}

void Application::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowClosedEvent>(BIND_EVENT_FN(Application::OnWindowClose));

    for (auto it = m_LayerStack->rbegin(); it != m_LayerStack->rend(); it++)
    {
        if (!(*it)->IsActive() && event.GetCategory() != EventCategory::Window)
            continue;

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

        for (auto layer : *m_LayerStack)
        {
            if (!layer->IsActive())
                continue;

            layer->OnUpdate(m_DeltaTime);
        }

        if (m_LayerStackReload != LayerStackReload::NONE)
            ProcessLayerStackReload();

        m_Window->OnUpdate();
    }
}

void Application::OpenMainMenu()
{
    m_LayerStackReload = LayerStackReload::OPEN_MAIN_MENU;
}

void Application::OpenMapEditor()
{
    m_LayerStackReload = LayerStackReload::OPEN_MAP_EDITOR;
}

void Application::StartNewGame(NewGameDTO newGameData)
{
    m_NewGameData = newGameData;
    m_LayerStackReload = LayerStackReload::START_NEW_GAME;
}

void Application::ContinueLastGame()
{
    m_LayerStackReload = LayerStackReload::CONTINUE_LAST_GAME;
}

void Application::LoadSave(const std::string& saveName)
{
    m_SaveName = saveName;
    m_LayerStackReload = LayerStackReload::LOAD_SAVE;
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

    ResourceManager::LoadTexture("wood", "assets/textures/resources/wood.png");
    ResourceManager::LoadTexture("rock", "assets/textures/resources/rock.png");
    ResourceManager::LoadTexture("steel", "assets/textures/resources/steel.png");
    ResourceManager::LoadTexture("gold", "assets/textures/resources/gold.png");

    ResourceManager::LoadTexture("cross", "assets/textures/icons/cross.png");
    ResourceManager::LoadTexture("up_arrow", "assets/textures/icons/up_arrow.png");
    ResourceManager::LoadTexture("chest_open", "assets/textures/icons/chest_open.png");
    ResourceManager::LoadTexture("chest_closed", "assets/textures/icons/chest_closed.png");

    ResourceManager::LoadTexture("target", "assets/textures/buildings/target.png");
    ResourceManager::LoadTexture("blacksmith", "assets/textures/buildings/blacksmith.png");
    ResourceManager::LoadTexture("gold_mine", "assets/textures/buildings/gold_mine.png");
    ResourceManager::LoadTexture("harpy_tower", "assets/textures/buildings/harpy_tower.png");
    ResourceManager::LoadTexture("demon_castle", "assets/textures/buildings/demon_castle.png");

    ResourceManager::LoadTexture("shield", "assets/textures/stats/shield.png");
    ResourceManager::LoadTexture("swords", "assets/textures/stats/swords.png");
    ResourceManager::LoadTexture("heart", "assets/textures/stats/heart.png");
}

void Application::InitializeColors()
{
    ColorData::Get().TileColors.MiniMapColor = { 0.2f, 0.2f, 0.2f, 1.0f };
    ColorData::Get().TileColors.TileHoverBorderColor = { 0.95f, 0.8f, 0.2f, 1.0f };
    ColorData::Get().TileColors.OceanColor = { 0.2f, 0.5f, 0.8f };
    ColorData::Get().TileColors.ForestColor = { 0.133f, 0.545f, 0.133f };
    ColorData::Get().TileColors.DesertColor = { 0.898f, 0.788f, 0.643f };
    ColorData::Get().TileColors.MountainsColor = { 0.5f, 0.5f, 0.5f };
    ColorData::Get().UITheme.ShopPanelBackgroundColor = { 0.4f, 0.4f, 0.4f, 1.0f };
    ColorData::Get().UITheme.ShopPanelHighlighUnitGroupColor = { 0.5f, 0.5f, 0.5f, 1.0f };
    ColorData::Get().Resources.Wood = { 0.545f, 0.270f, 0.075f };
    ColorData::Get().Resources.Rock = { 0.5f, 0.5f, 0.5f };
    ColorData::Get().Resources.Steel = { 0.7f, 0.7f, 0.7f };
    ColorData::Get().Resources.Gold = { 0.984f, 0.792f, 0.188f };
}

void Application::ProcessLayerStackReload()
{
    switch (m_LayerStackReload)
    {
        case LayerStackReload::OPEN_MAIN_MENU:
        {
            m_MainMenuLayer->SetIsActive(true);
            m_MainMenuLayer->OnAttach();
            if (m_GameLayer)
                m_GameLayer->SetIsActive(false);
            if (m_UILayer)
                m_UILayer->SetIsActive(false);
            if (m_EditorLayer)
                m_EditorLayer->SetIsActive(false);
#if defined(DEBUG)
            if (m_DebugLayer)
                m_DebugLayer->SetIsActive(false);
#endif
            break;
        }
        case LayerStackReload::START_NEW_GAME:
        {
            m_MainMenuLayer->SetIsActive(false);

            m_GameLayer = std::make_shared<GameLayer>();
            m_GameLayer->OnAttach();
            m_GameLayer->InitGame(m_NewGameData);
            m_LayerStack->PushLayer(m_GameLayer);

            m_UILayer = std::make_shared<UILayer>();
            m_UILayer->OnAttach();
            m_LayerStack->PushOverlay(m_UILayer);

#if defined(DEBUG)
            m_DebugLayer = std::make_shared<DebugLayer>();
            m_DebugLayer->OnAttach();
            m_LayerStack->PushOverlay(m_DebugLayer);
#endif

            m_LastGameLayer = m_GameLayer;
            break;
        }
        case LayerStackReload::CONTINUE_LAST_GAME:
        {
            if (m_LastGameLayer)
                m_GameLayer = m_LastGameLayer;

            m_MainMenuLayer->SetIsActive(false);
            m_GameLayer->SetIsActive(true);
            m_UILayer->SetIsActive(true);
#if defined(DEBUG)
            m_DebugLayer->SetIsActive(true);
#endif
            break;
        }
        case LayerStackReload::LOAD_SAVE:
        {
            try
            {
                m_GameLayer = SaveLoader::Load(m_SaveName);
                m_GameLayer->OnAttach();
                m_LayerStack->PushLayer(m_GameLayer);

                m_MainMenuLayer->SetIsActive(false);
            }
            catch (SaveLoaderException e)
            {
                // TODO: Add UI notification
                LOG_ERROR("Corrupted .war file: {0}", e.what());
                break;
            }

            m_UILayer = std::make_shared<UILayer>();
            m_UILayer->OnAttach();
            m_LayerStack->PushOverlay(m_UILayer);

#if defined(DEBUG)
            m_DebugLayer = std::make_shared<DebugLayer>();
            m_DebugLayer->OnAttach();
            m_LayerStack->PushOverlay(m_DebugLayer);
#endif

            m_LastGameLayer = m_GameLayer;
            break;
        }
        case LayerStackReload::OPEN_MAP_EDITOR:
        {
            m_MainMenuLayer->SetIsActive(false);
            m_EditorLayer = std::make_shared<EditorLayer>();
            m_EditorLayer->OnAttach();
            m_LayerStack->PushLayer(m_EditorLayer);
            break;
        }
        default:
            break;
    }

    m_LayerStackReload = LayerStackReload::NONE;
}
