#pragma once

#include <memory>
#include <variant>
#include <optional>
#include <unordered_map>

#include "core/camera.h"
#include "ui/ui_element.h"
#include "game/unit.h"
#include "game/building.h"
#include "event/mouse_event.h"
#include "event/window_event.h"
#include "event/key_event.h"
#include "graphics/texture.h"

class ShopPanel : public UIElement
{
public:
    ShopPanel(const std::shared_ptr<OrthographicCamera>& UICamera, const glm::vec2& offset,
              const glm::vec2& unitSize = glm::vec2(0.15f), float unitOffset = 0.05f);
    ~ShopPanel() = default;

    virtual void OnEvent(Event& event) override;
    virtual void Draw() override;

private:
    void OnWindowSizeChanged();

    bool OnKeyPressed(KeyPressedEvent& event);
    bool OnMouseScrolled(MouseScrolledEvent& event);
    bool OnMouseButtonPressedPanel(MouseButtonPressedEvent& event);
    bool OnMouseButtonShopPanelIconPressed(MouseButtonPressedEvent& event);
    bool OnMouseButtonPressedGame(MouseButtonPressedEvent& event);
    void SetCursorAttachedAsset(std::variant<UnitGroupType, BuildingType> type);
    bool IsAssetAttachedToCursor() { return m_CursorAttachedAsset.Texture.get(); }
    void ProcessInvalidAssetPlacement(const glm::vec2& cursorPos);

    void DrawShopPanelIcon(const glm::vec2& cursorPos);
    void DrawUnitGroups(const glm::vec2& cursorPos);
    void DrawBuildings(const glm::vec2& cursorPos);
    void DrawAssetInfo(const std::string& name, const Resources& cost,
                       std::optional<BuildingType> requiredBuilding = std::nullopt);

    std::string GetCostText(Resources& cost);

private:
    int m_UnitGroupCount;
    int m_BuildingCount;
    float m_AssetOffset;
    float m_AssetBorderMargin;
    float m_AssetBorderThickness;
    glm::vec2 m_AssetSize;
    float m_AssetPriceSize;
    std::string m_AssetPriceFontName;
    glm::vec2 m_Offset;
    bool m_Hidden;

    struct
    {
        glm::vec2 CornerOffset;
        glm::vec2 Size;
        glm::vec2 Position;
    } m_ShopPanelIcon;

    struct
    {
        UnitGroupType UnitGroupType = UnitGroupType::NONE;
        BuildingType BuildingType = BuildingType::NONE;
        std::shared_ptr<Texture2D> Texture;
    } m_CursorAttachedAsset;
};
