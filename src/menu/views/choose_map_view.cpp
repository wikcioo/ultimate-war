#include "choose_map_view.h"

#include "util/util.h"
#include "graphics/renderer.h"
#include "menu/main_menu_layer.h"
#include "game/map_manager.h"
#include "core/application.h"

ChooseMapView::ChooseMapView()
    : m_SelectedMap("")
{
}

ChooseMapView::~ChooseMapView()
{
}

void ChooseMapView::OnAttach()
{
    BackableView::OnAttach();
    m_Buttons.clear();
    auto maps = GameMapManager::GetAvailableMaps();

    static float yOffset = 0.05f;
    static float buttonHeight = 0.1f;
    float totalHeight = (maps.size() * buttonHeight) + ((maps.size() - 1) * yOffset);
    float yStartPosition = totalHeight / 2.0f - buttonHeight / 2.0f;

    for (const auto& map : maps)
    {
        Button* mapButton = new Button(
            m_Camera,
            { Util::StrToUpper(map), glm::vec2(0.0f, yStartPosition), glm::vec2(1.0f, 0.1f) }
        );
        mapButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(ChooseMapView::OnMapButtonPressed));
        m_Buttons.emplace_back(mapButton);

        yStartPosition -= buttonHeight + yOffset;
    }
}

void ChooseMapView::OnUpdate(float dt)
{
    for (auto button : m_Buttons)
        button->OnUpdate();

    BackableView::OnUpdate(dt);
}

void ChooseMapView::OnEvent(Event& event)
{
    for (auto button : m_Buttons)
        button->OnEvent(event);

    BackableView::OnEvent(event);
}

void ChooseMapView::OnMapButtonPressed(ButtonCallbackData data)
{
    m_SelectedMap = Util::StrToLower(data.Text);

    std::vector<PlayerDTO> players;
    players.emplace_back(PlayerDTO("John", glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(4)));
    players.emplace_back(PlayerDTO("Bob", glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(5)));

    Application::Get().StartNewGame({ m_SelectedMap, players });
}
