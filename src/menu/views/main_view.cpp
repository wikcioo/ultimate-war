#include "main_view.h"

#include "core/application.h"
#include "graphics/renderer.h"

MainView::MainView()
{
    Button* startNewGameButton = new Button(
        m_Camera,
        { "START NEW GAME", glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.1f) }
    );
    startNewGameButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(MainView::OnStartNewGameButtonPressed));

    Button* continueLastGameButton = new Button(
        m_Camera,
        { "CONTINUE LAST GAME", glm::vec2(0.0f, -0.15f), glm::vec2(1.0f, 0.1f) }
    );
    continueLastGameButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(MainView::OnContinueLastGameButtonPressed));

    Button* mapEditorButton = new Button(
        m_Camera,
        { "MAP EDITOR", glm::vec2(0.0f, -0.3f), glm::vec2(1.0f, 0.1f) }
    );
    mapEditorButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(MainView::OnOpenMapEditorButtonPressed));

    Button* showContributorsButton = new Button(
        m_Camera,
        { "CONTRIBUTORS", glm::vec2(0.0f, -0.45f), glm::vec2(1.0f, 0.1f) }
    );
    showContributorsButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(MainView::OnShowContributorsButtonPressed));

    Button* exitButton = new Button(
        m_Camera,
        { "EXIT", glm::vec2(0.0f, -0.6f), glm::vec2(1.0f, 0.1f) }
    );
    exitButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(MainView::OnExitButtonPressed));

    m_Buttons.emplace_back(startNewGameButton);
    m_Buttons.emplace_back(continueLastGameButton);
    m_Buttons.emplace_back(mapEditorButton);
    m_Buttons.emplace_back(showContributorsButton);
    m_Buttons.emplace_back(exitButton);
}

MainView::~MainView()
{
    for (auto button : m_Buttons)
        delete button;
}

void MainView::OnAttach()
{
    m_Buttons[1]->SetDisabled(!Application::Get().LastGameAvailable());
}

void MainView::OnUpdate(float dt)
{
    Renderer2D::DrawTextStr("Ultimate War", glm::vec2(0.0f, 0.5f), 1.0f, glm::vec3(0.9f),
                            HTextAlign::MIDDLE, VTextAlign::MIDDLE, "rexlia");

    for (auto button : m_Buttons)
        button->OnUpdate();
}

void MainView::OnEvent(Event& event)
{
#if defined(DEBUG)
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(MainView::OnKeyPressed));
#endif

    for (auto button : m_Buttons)
    {
        if (event.Handled)
            break;
        button->OnEvent(event);
    }
}

void MainView::OnStartNewGameButtonPressed(ButtonCallbackData data)
{
    MainMenuLayer::Get().SetView(ViewName::CHOOSE_MAP);
}

void MainView::OnContinueLastGameButtonPressed(ButtonCallbackData data)
{
    Application::Get().ContinueLastGame();
}

void MainView::OnOpenMapEditorButtonPressed(ButtonCallbackData data)
{
    Application::Get().OpenMapEditor();
}

void MainView::OnShowContributorsButtonPressed(ButtonCallbackData data)
{
    MainMenuLayer::Get().SetView(ViewName::CONTRIBUTORS);
}

void MainView::OnExitButtonPressed(ButtonCallbackData data)
{
    Application::Get().Exit();
}

#if defined(DEBUG)
#include <algorithm>
// quick way to get into the game layer from main menu layer
// adds two dummy players to the first available tiles on the map
// looks for map named 'simple' to load, otherwise loads the first available
bool MainView::OnKeyPressed(KeyPressedEvent& event)
{
    if (event.GetKeyCode() == GLFW_KEY_F1)
    {
        auto availableMaps = GameMapManager::GetAvailableMaps();
        if (availableMaps.size() < 1)
        {
            LOG_WARN("No available maps to load");
            return false;
        }

        std::string mapName;
        if (std::find(availableMaps.begin(), availableMaps.end(), "simple") != availableMaps.end())
            mapName = "simple";
        else
            mapName = availableMaps[0];

        GameMapManager manager(mapName);
        std::vector<PlayerDTO> players;
        std::vector<glm::vec2> tileCoords;

        for (int y = 0; y < manager.GetGameMap()->GetTileCountY(); y++)
        {
            for (int x = 0; x < manager.GetGameMap()->GetTileCountX(); x++)
            {
                auto tile = manager.GetGameMap()->GetTile(x, y);
                if (tile->AssetsCanExist())
                {
                    tileCoords.emplace_back(tile->GetCoords());
                    if (tileCoords.size() == 2)
                    {
                        players.emplace_back(PlayerDTO("Foo", glm::vec3(1.0f, 0.0f, 0.0f), tileCoords[0]));
                        players.emplace_back(PlayerDTO("Bar", glm::vec3(0.0f, 0.0f, 1.0f), tileCoords[1]));
                        Application::Get().StartNewGame({ mapName, players });
                        return true;
                    }
                }
            }
        }

        LOG_WARN("Not enough available tiles to place players on");
    }

    return false;
}
#endif
