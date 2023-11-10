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

    Button* chooseMapButton = new Button(
        m_Camera,
        { "CHOOSE THE MAP", glm::vec2(0.0f, -0.15f), glm::vec2(1.0f, 0.1f) }
    );
    chooseMapButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(MainView::OnChooseMapButtonPressed));

    Button* showContributorsButton = new Button(
        m_Camera,
        { "CONTRIBUTORS", glm::vec2(0.0f, -0.3f), glm::vec2(1.0f, 0.1f) }
    );
    showContributorsButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(MainView::OnShowContributorsButtonPressed));

    Button* exitButton = new Button(
        m_Camera,
        { "EXIT", glm::vec2(0.0f, -0.45f), glm::vec2(1.0f, 0.1f) }
    );
    exitButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(MainView::OnExitButtonPressed));

    m_Buttons.emplace_back(startNewGameButton);
    m_Buttons.emplace_back(chooseMapButton);
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
}

void MainView::OnUpdate()
{
    Renderer2D::DrawTextStr("Ultimate War", glm::vec2(0.0f, 0.5f), 1.0f, glm::vec3(0.9f),
                            HTextAlign::MIDDLE, VTextAlign::MIDDLE, "rexlia");

    for (auto button : m_Buttons)
        button->OnUpdate();
}

void MainView::OnEvent(Event& event)
{
    for (auto button : m_Buttons)
    {
        if (event.Handled)
            break;
        button->OnEvent(event);
    }
}

void MainView::OnStartNewGameButtonPressed(ButtonCallbackData data)
{
    auto selectedMap = MainMenuLayer::Get().GetSelectedMap();
    if (selectedMap.empty())
    {
        LOG_WARN("No map selected");
        return;
    }

    std::vector<PlayerDTO> players;
    players.emplace_back((PlayerDTO){ "John", glm::vec3(1.0f, 0.0f, 0.0f) });
    players.emplace_back((PlayerDTO){ "Bob", glm::vec3(0.0f, 0.0f, 1.0f) });

    Application::Get().StartNewGame({ selectedMap, players });
}

void MainView::OnChooseMapButtonPressed(ButtonCallbackData data)
{
    MainMenuLayer::Get().SetView(ViewName::CHOOSE_MAP);
}

void MainView::OnShowContributorsButtonPressed(ButtonCallbackData data)
{
    MainMenuLayer::Get().SetView(ViewName::CONTRIBUTORS);
}

void MainView::OnExitButtonPressed(ButtonCallbackData data)
{
    Application::Get().Exit();
}
