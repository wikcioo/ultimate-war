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
    m_Buttons.emplace_back(startNewGameButton);
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
    // NOTE: Hardcoded until choose map view implemented
    auto selectedMap = "environments";

    std::vector<PlayerDTO> players;
    players.emplace_back((PlayerDTO){ "John", glm::vec3(1.0f, 0.0f, 0.0f) });
    players.emplace_back((PlayerDTO){ "Bob", glm::vec3(0.0f, 0.0f, 1.0f) });

    Application::Get().StartNewGame({ selectedMap, players });
}
