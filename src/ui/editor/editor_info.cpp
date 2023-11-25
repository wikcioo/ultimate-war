#include "editor_info.h"

#include <string>

#include "graphics/renderer.h"
#include "editor/editor_layer.h"
#include "util/util.h"

EditorInfo::EditorInfo(const std::shared_ptr<OrthographicCamera>& UICamera, const glm::vec2& offset, const glm::vec2& size)
    : UIElement(UICamera, UICamera->CalculateRelativeBottomLeftPosition() + offset, {size.y * UICamera->GetAspectRatio(), size.y})
{
}

void EditorInfo::Draw()
{
    auto halfOfWidth = m_UICamera->GetHalfOfRelativeWidth();
    auto halfOfHeight = m_UICamera->GetHalfOfRelativeHeight();
    static float textScale = 0.3f;

    static std::string mapEditor = "Map Editor";
    static std::string save = "Press X to save";

    std::string selectedEnvironment = Tile::GetEnvironmentName(EditorLayer::Get().GetSelectedTileEnvType());
    Renderer2D::BeginScene(m_UICamera);

    Renderer2D::DrawTextStr(selectedEnvironment, {  halfOfWidth - 0.05f, halfOfHeight - 0.1f }, textScale, glm::vec3(0.9f), HTextAlign::RIGHT);
    Renderer2D::DrawTextStr(save, {  -halfOfWidth + 0.05f, halfOfHeight - 0.1f }, textScale, glm::vec3(0.9f), HTextAlign::LEFT);
    Renderer2D::DrawTextStr(mapEditor, { 0.0f, halfOfHeight - 0.1f }, textScale, glm::vec3(0.9f), HTextAlign::MIDDLE);

    Renderer2D::EndScene();
}
