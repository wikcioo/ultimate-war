#pragma once

#include <map>
#include <memory>

#include "game/player.h"
#include "game/map_manager.h"
#include "graphics/buffer.h"
#include "core/camera_controller.h"
#include "menu/views/backable_view.h"
#include "widgets/button.h"
#include "widgets/input_box.h"

struct Vec2FloatCompare {
    bool operator()(const glm::vec2& a, const glm::vec2& b) const {
        return a.x < b.x || (!(b.x < a.x) && a.y < b.y);
    }
};

enum class ColumnType
{
    INFO,
    MAP
};

struct ColumnDrawData
{
    glm::vec2 Position;
    glm::vec2 Size;
};

struct PlayerData
{
    PlayerDTO Player;
    std::string TileEnvName;

    PlayerData() {}

    PlayerData(PlayerDTO player, const std::string& tileEnvName)
    {
        Player = player;
        TileEnvName = tileEnvName;
    }
};

class ChoosePlayersView : public BackableView
{
public:
    ChoosePlayersView();
    ~ChoosePlayersView() = default;

    virtual void OnAttach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnEvent(Event& event) override;

private:
    bool OnKeyPressed(KeyPressedEvent& event);
    bool OnMouseButtonPressed(MouseButtonPressedEvent& event);

    void OnStartGameButtonPressed(ButtonCallbackData data);
    void OnAddPlayerButtonPressed(ButtonCallbackData data);
    void OnAddComputerPlayerButtonPressed(ButtonCallbackData data);
    void OnUsernameInputBoxAccepted(InputBoxCallbackData data);

    void AddPlayer(bool computerPlayer = false);

    void DrawInfoColumn();
    void DrawAddPlayerInfo();
    void DrawPlayerListInfo();
    void DrawMapPreviewColumn(float dt);

    void OnWindowSizeChanged();
    ColumnDrawData GetColumnDrawData(ColumnType type);
    glm::vec2 CalculateMousePositionOnMap();
    glm::vec2 CalculateStartGameButtonPosition();

private:
    struct
    {
        float WidthRatio;
        float VMargin;
    } m_ColumnData;

    ColumnDrawData m_InfoDrawData;
    ColumnDrawData m_MapDrawData;

    struct
    {
        bool Selected = false;
        std::shared_ptr<Tile> TileRef;
    } m_SelectedTile;

    struct
    {
        glm::vec3 Color;
        glm::vec2 OwnedTileCoords;
    } m_NextPlayerInfo;

    std::string m_PlayerUsername;
    std::map<glm::vec2, PlayerData, Vec2FloatCompare> m_PlayersData;

    std::unique_ptr<InputBox> m_UsernameInputBox;
    std::unique_ptr<Button> m_AddPlayerButton;
    std::unique_ptr<Button> m_AddComputerPlayerButton;
    std::unique_ptr<Button> m_StartGameButton;
    glm::vec2 m_StartGameButtonSize;

    glm::vec2 m_MapZoom;

    std::unique_ptr<FrameBuffer> m_Framebuffer;
    std::shared_ptr<GameMapManager> m_GameMapManager;
    std::shared_ptr<OrthographicCameraController> m_MapCameraController;
};
