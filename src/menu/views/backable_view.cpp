#include "backable_view.h"

#include "menu/main_menu_layer.h"

BackableView::BackableView(ViewName backViewName)
    : m_BackButtonSize(0.4, 0.1), m_BackViewName(backViewName)
{
    ButtonConfig buttonConfig;
    buttonConfig.Text = "BACK";
    buttonConfig.Position = CalculateBackButtonPosition();
    buttonConfig.Size = m_BackButtonSize;

    m_BackButton = std::make_unique<Button>(m_Camera, buttonConfig);
    m_BackButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(BackableView::OnBackButtonPressed));
}

BackableView::~BackableView()
{
}

void BackableView::OnAttach()
{
    m_BackButton->SetPosition(CalculateBackButtonPosition());
}

void BackableView::OnUpdate(float dt)
{
    m_BackButton->OnUpdate();
}

void BackableView::OnEvent(Event& event)
{
    if (event.GetCategory() == EventCategory::Window)
        OnWindowSizeChanged();

    m_BackButton->OnEvent(event);
}

void BackableView::OnWindowSizeChanged()
{
    m_BackButton->SetPosition(CalculateBackButtonPosition());
}

void BackableView::OnBackButtonPressed(ButtonCallbackData data)
{
    MainMenuLayer::Get().SetView(m_BackViewName);
}

glm::vec2 BackableView::CalculateBackButtonPosition()
{
    const glm::vec2& relWindowSize = MainMenuLayer::Get().GetRelWindowSize();
    return glm::vec2(
        relWindowSize.x / 2.0f - (m_BackButtonSize.x),
        -relWindowSize.y / 2.0f + (m_BackButtonSize.y * 2)
    );
}
