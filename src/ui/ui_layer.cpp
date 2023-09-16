#include "ui_layer.h"

#include "core/core.h"
#include "core/input.h"
#include "core/application.h"

UILayer::UILayer(const UILayerData& data)
    : Layer("UILayer"), m_GameCamera(data._GameCamera)
{
    m_UICamera = std::make_shared<OrthographicCamera>(m_GameCamera->GetAspectRatio());
    m_UIElements.emplace_back(std::make_shared<Minimap>(m_UICamera, m_GameCamera, data._GameMap));
}

void UILayer::OnAttach()
{
}

void UILayer::OnDetach()
{
}

void UILayer::OnUpdate(float dt)
{
    for (auto element : m_UIElements)
        element->Draw();
}

glm::vec2 UILayer::CalculateScreenRelativeMousePosition()
{
    auto mousePos = Input::GetMousePosition();

    static auto window = Application::Get().GetWindow();
    float pixelWidth = (float)window->GetWidth();
    float pixelHeight = (float)window->GetHeight();

    float relWidth = m_UICamera->GetZoom() * m_UICamera->GetAspectRatio() * 2;
    float relHeight = m_UICamera->GetZoom() * 2;

    float relX = (mousePos.x * relWidth / pixelWidth) - m_UICamera->GetZoom() * m_UICamera->GetAspectRatio();
    float relY = ((mousePos.y * relHeight / pixelHeight) - m_UICamera->GetZoom()) * -1;

    return { relX, relY };
}

void UILayer::OnEvent(Event& event)
{
    for (auto element : m_UIElements)
    {
        auto mousePos = CalculateScreenRelativeMousePosition();
        auto elementPos = element->GetPosition();
        auto elementSize = element->GetSize();

        if (mousePos.x > elementPos.x && mousePos.x < elementPos.x + elementSize.x &&
            mousePos.y > elementPos.y && mousePos.y < elementPos.y + elementSize.y)
        {
            element->OnEvent(event);
        }
    }
}
