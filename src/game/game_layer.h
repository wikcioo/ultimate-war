#pragma once

#include <memory>

#include "core/camera_controller.h"
#include "layer/layer.h"
#include "event/window_event.h"
#include "event/mouse_event.h"
#include "event/key_event.h"
#include "graphics/shader.h"
#include "graphics/buffer.h"
#include "graphics/texture.h"
#include "graphics/vertex_array.h"
#include "game/map_manager.h"
#include "game/arrow.h"
#include "game/player_manager.h"
#include "game/color_data.h"

struct NewGameDTO
{
    std::string MapName;
    std::vector<PlayerDTO> Players;
};

class GameLayer : public Layer
{
#if defined(DEBUG)
    friend class DebugLayer;
#endif
public:
    GameLayer();
    ~GameLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnEvent(Event& event) override;

    static GameLayer& Get() { return *s_Instance; }

    inline const std::shared_ptr<OrthographicCameraController>& GetCameraController() const { return m_CameraController; }
    inline const std::shared_ptr<GameMapManager>& GetGameMapManager() const { return m_GameMapManager; }
    inline const std::shared_ptr<PlayerManager>& GetPlayerManager() const { return m_PlayerManager; }
    inline bool IsGameActive() const { return m_GameActive; }
    inline int GetIteration() { return m_IterationNumber; }

    void InitGame(NewGameDTO newGameData);
    void NextIteration() { m_IterationNumber++; }
    void ResetArrow();
    void EndGame();

private:
    bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
    bool OnKeyReleased(KeyReleasedEvent& event);
    void ProcessTileInRange(const std::shared_ptr<Tile>& tile, const std::shared_ptr<Player>& currentPlayer, const glm::vec2& relMousePos);

private:
    static GameLayer* s_Instance;

    std::shared_ptr<OrthographicCameraController> m_CameraController;
    std::shared_ptr<GameMapManager> m_GameMapManager;
    std::shared_ptr<PlayerManager> m_PlayerManager;
    std::shared_ptr<Arrow> m_Arrow;
    int m_IterationNumber;
    bool m_GameActive;
};
