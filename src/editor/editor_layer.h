#pragma once

#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "core/camera_controller.h"
#include "event/mouse_event.h"
#include "event/key_event.h"
#include "layer/layer.h"
#include "game/tile.h"

class EditorLayer : public Layer
{
public:
    EditorLayer();
    ~EditorLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnEvent(Event& event) override;

    static EditorLayer& Get() { return *s_Instance; }
private:
    bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
    bool OnKeyPressed(KeyPressedEvent& event);
    bool OnMouseMoved(MouseMovedEvent& event);

    void CreateAdjacentHightlightTiles(glm::ivec2 coords);
    void UpdateTile(Tile* tile);
    void SaveMap();
    void RemoveTileWithNoAdjacent(Tile* tile);
    void CheckAdjacentTilesForRemoval(Tile* tile);
    void CheckForTileInRange();

private:
    std::shared_ptr<OrthographicCamera> m_UICamera;
    static EditorLayer* s_Instance;

    std::shared_ptr<OrthographicCameraController> m_CameraController;
    std::unordered_map<glm::ivec2, Tile*> m_Map;
    Tile* m_PreviousTile;
    TileEnvironment m_SelectedTileEnvType;
};
