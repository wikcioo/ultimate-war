#include "choose_map_view.h"

#include "util/util.h"
#include "graphics/renderer.h"
#include "menu/main_menu_layer.h"
#include "game/map_manager.h"

ChooseMapView::ChooseMapView()
    : m_SelectedMap("")
{
    m_MapButtonGroup = std::make_unique<ToggleButtonGroup>(m_Camera);

    auto maps = GameMapManager::GetAvailableMaps();

    float yOffset = 0.05f;
    float buttonHeight = 0.1f;
    float totalHeight = (maps.size() * buttonHeight) + ((maps.size() - 1) * yOffset);
    float yStartPosition = totalHeight / 2.0f - buttonHeight / 2.0f;

    for (const auto& map : maps)
    {
        m_MapButtonGroup->AddButton({
            Util::StrToUpper(map), glm::vec2(0.0f, yStartPosition), glm::vec2(0.8f, buttonHeight)
        });
        yStartPosition -= buttonHeight + yOffset;
    }

    m_MapButtonGroup->SetPressedCallback(BIND_BTN_CALLBACK_FN(ChooseMapView::OnMapButtonPressed));
}

ChooseMapView::~ChooseMapView()
{
}

void ChooseMapView::OnUpdate()
{
    m_MapButtonGroup->OnUpdate();

    BackableView::OnUpdate();
}

void ChooseMapView::OnEvent(Event& event)
{
    m_MapButtonGroup->OnEvent(event);

    BackableView::OnEvent(event);
}

void ChooseMapView::OnMapButtonPressed(ButtonCallbackData data)
{
    m_SelectedMap = Util::StrToLower(data.Text);
}
