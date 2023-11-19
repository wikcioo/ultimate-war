#include "choose_players_view.h"

#include "util/util.h"
#include "core/logger.h"
#include "game/color_data.h"
#include "graphics/renderer.h"
#include "core/application.h"

static const std::string VIEW_FONT = "rexlia";

ChoosePlayersView::ChoosePlayersView()
    : BackableView(ViewName::CHOOSE_MAP), m_ColumnData({0.5f, 0.35f}), m_PlayerUsername(""), m_StartGameButtonSize(0.4f, 0.1f)
{
    m_InfoDrawData = GetColumnDrawData(ColumnType::INFO);
    m_MapDrawData = GetColumnDrawData(ColumnType::MAP);

    m_GameMapManager = std::make_shared<GameMapManager>("");
    m_MapCameraController = std::make_shared<OrthographicCameraController>(m_MapDrawData.Size.x / m_MapDrawData.Size.y);
    auto mapPixelSize = m_Camera->ConvertRelativeSizeToPixel(m_MapDrawData.Size);
    m_Framebuffer = std::make_unique<FrameBuffer>((unsigned int)mapPixelSize.x, (unsigned int)mapPixelSize.y);

    m_NextPlayerInfo.Color = Util::GetRandomColor();

    ButtonConfig addPlayerButtonConfig = { "Add", glm::vec2(0.0f), glm::vec2(0.15f, 0.06f) };
    m_AddPlayerButton = std::make_unique<Button>(m_Camera, addPlayerButtonConfig);
    m_AddPlayerButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(ChoosePlayersView::OnAddPlayerButtonPressed));

    ButtonConfig startGameButtonConfig = { "Start", CalculateStartGameButtonPosition(), m_StartGameButtonSize };
    m_StartGameButton = std::make_unique<Button>(m_Camera, startGameButtonConfig);
    m_StartGameButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(ChoosePlayersView::OnStartGameButtonPressed));

    InputBoxConfig usernameInputBoxConfig = { glm::vec2(0.0f), glm::vec2(0.3f, 0.06f) };
    m_UsernameInputBox = std::make_unique<InputBox>(m_Camera, usernameInputBoxConfig);
    m_UsernameInputBox->SetAcceptedCallback(BIND_INPUT_BOX_CALLBACK_FN(ChoosePlayersView::OnUsernameInputBoxAccepted));
    m_UsernameInputBox->SetOnKeyChangedCallback(BIND_INPUT_BOX_CALLBACK_FN(ChoosePlayersView::OnUsernameInputBoxKeyChanged));
    m_UsernameInputBox->SetCharacterLimit(15);
}

void ChoosePlayersView::OnAttach()
{
    std::string mapName = MainMenuLayer::Get().GetSelectedMap();
    m_GameMapManager->Load(mapName);

    glm::vec2 mapMiddle = {
        (m_GameMapManager->GetGameMap()->GetTileCountX() * (3.0f / 4.0f * TILE_WIDTH + TILE_OFFSET) / 2.0f) - TILE_WIDTH / 2.0f,
        (m_GameMapManager->GetGameMap()->GetTileCountY() * (TILE_HEIGHT + TILE_OFFSET) / 2.0f) - (TILE_HEIGHT + TILE_OFFSET) / 4.0f
    };

    m_MapCameraController->GetCamera()->SetPosition(glm::vec3(mapMiddle, 0.0f));

    m_MapZoom = {
        (m_GameMapManager->GetGameMap()->GetTileCountX() * (3.0f / 4.0f * TILE_WIDTH + TILE_OFFSET) + TILE_WIDTH) / 2.0f,
        (m_GameMapManager->GetGameMap()->GetTileCountY() * (TILE_HEIGHT + TILE_OFFSET) + (TILE_HEIGHT + TILE_OFFSET)) / 2.0f
    };

    OnWindowSizeChanged();

    m_PlayersData.clear();
    m_UsernameInputBox->Clear();
    m_SelectedTile.Selected = false;
    m_SelectedTile.TileRef = nullptr;

    BackableView::OnAttach();
}

void ChoosePlayersView::OnUpdate(float dt)
{
    Renderer2D::DrawTextStr(
        "Choose players",
        { 0.0f, m_Camera->GetHalfOfRelativeHeight() - 0.1f },
        0.7f,
        glm::vec3(1.0f),
        HTextAlign::MIDDLE,
        VTextAlign::TOP,
        VIEW_FONT
    );

    DrawInfoColumn();
    DrawMapPreviewColumn(dt);

    BackableView::OnUpdate(dt);
}

void ChoosePlayersView::OnEvent(Event& event)
{
    m_MapCameraController->OnEvent(event);

    if (event.GetCategory() == EventCategory::Window)
        OnWindowSizeChanged();

    m_StartGameButton->OnEvent(event);

    if (m_SelectedTile.Selected)
    {
        m_AddPlayerButton->OnEvent(event);
        m_UsernameInputBox->OnEvent(event);
    }

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(ChoosePlayersView::OnKeyPressed));
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(ChoosePlayersView::OnMouseButtonPressed));

    BackableView::OnEvent(event);
}

bool ChoosePlayersView::OnKeyPressed(KeyPressedEvent& event)
{
    if (event.GetKeyCode() == GLFW_KEY_ESCAPE)
    {
        m_SelectedTile.Selected = false;
        m_SelectedTile.TileRef = nullptr;
        m_UsernameInputBox->SetFocused(false);
        return true;
    }

    return false;
}

bool ChoosePlayersView::OnMouseButtonPressed(MouseButtonPressedEvent& event)
{
    glm::vec2 relMousePos = m_Camera->CalculateRelativeMousePosition();
    if (!Util::IsPointInRectangle(m_MapDrawData.Position, m_MapDrawData.Size, relMousePos))
        return false;

    glm::vec2 mapMousePos = CalculateMousePositionOnMap();

    for (int y = 0; y < m_GameMapManager->GetGameMap()->GetTileCountY(); y++)
    {
        for (int x = 0; x < m_GameMapManager->GetGameMap()->GetTileCountX(); x++)
        {
            auto tile = m_GameMapManager->GetGameMap()->GetTile(x, y);

            if (tile->InRange(mapMousePos) && tile->AssetsCanExist())
            {
                m_SelectedTile.Selected = true;
                m_SelectedTile.TileRef = tile;
                m_NextPlayerInfo.Color = Util::GetRandomColor();
                m_UsernameInputBox->SetFocused(true);
                for (auto player : m_PlayersData)
                {
                    if (player.first == tile->GetPosition())
                        m_UsernameInputBox->SetText(player.second.Player.Name);
                }

                return true;
            }
        }
    }

    m_SelectedTile.Selected = false;
    m_SelectedTile.TileRef = nullptr;

    return false;
}

void ChoosePlayersView::OnStartGameButtonPressed(ButtonCallbackData data)
{
    if (m_PlayersData.size() < 2)
    {
        LOG_WARN("ChoosePlayersView: At least 2 players needed to start the game");
        return;
    }

    std::string mapName = MainMenuLayer::Get().GetSelectedMap();
    std::vector<PlayerDTO> players;
    for (auto it = m_PlayersData.begin(); it != m_PlayersData.end(); it++)
    {
        std::string name = it->second.Player.Name;
        glm::vec3 color = it->second.Player.Color;
        std::vector<glm::vec2> tileCoords = it->second.Player.TileCoords;
        players.emplace_back(PlayerDTO(name, color, tileCoords));
    }

    Application::Get().StartNewGame({ mapName, players });
}

void ChoosePlayersView::OnAddPlayerButtonPressed(ButtonCallbackData data)
{
    AddPlayer();
}

void ChoosePlayersView::OnUsernameInputBoxAccepted(InputBoxCallbackData data)
{
    AddPlayer();
}

void ChoosePlayersView::OnUsernameInputBoxKeyChanged(InputBoxCallbackData data)
{
    m_UsernameInputBox->SetText(data.Text);
}

void ChoosePlayersView::AddPlayer()
{
    if (m_UsernameInputBox->GetText().empty())
    {
        LOG_WARN("ChoosePlayersView: Empty username");
        return;
    }

    auto key = m_SelectedTile.TileRef->GetPosition();
    if (m_PlayersData.find(key) != m_PlayersData.end())
    {
        // allow to rename and change color of the player if it already exists in players data
        m_PlayersData[key].Player.Name = m_UsernameInputBox->GetText();
        m_PlayersData[key].Player.Color = m_NextPlayerInfo.Color;
    }
    else
    {
        m_PlayersData.insert(std::make_pair(
            m_SelectedTile.TileRef->GetPosition(),
            PlayerData(
                PlayerDTO(m_UsernameInputBox->GetText(), m_NextPlayerInfo.Color, { m_SelectedTile.TileRef->GetCoords() }),
                m_SelectedTile.TileRef->GetEnvironmentName(m_SelectedTile.TileRef->GetEnvironment())
            )
        ));
    }

    m_NextPlayerInfo.Color = Util::GetRandomColor();
    m_SelectedTile.Selected = false;
    m_SelectedTile.TileRef = nullptr;
    m_UsernameInputBox->Clear();
}

void ChoosePlayersView::DrawInfoColumn()
{
    DrawAddPlayerInfo();
    DrawPlayerListInfo();

    m_StartGameButton->OnUpdate();
}

void ChoosePlayersView::DrawAddPlayerInfo()
{
    Renderer2D::DrawTextStr(
        "Add player",
        glm::vec2(
            m_InfoDrawData.Position.x,
            m_InfoDrawData.Position.y + m_InfoDrawData.Size.y / 2.0f - 0.05f
        ),
        0.5f,
        glm::vec3(1.0f),
        HTextAlign::MIDDLE,
        VTextAlign::TOP,
        VIEW_FONT
    );

    if (!m_SelectedTile.Selected)
    {
        Renderer2D::DrawTextStr(
            "select tile",
            glm::vec2(
                m_InfoDrawData.Position.x,
                m_InfoDrawData.Position.y + m_InfoDrawData.Size.y / 2.0f - 0.3f
            ),
            0.3f,
            glm::vec3(1.0f),
            HTextAlign::MIDDLE,
            VTextAlign::MIDDLE,
            VIEW_FONT
        );

        return;
    }

    float addPlayerInfoYPosition = m_InfoDrawData.Position.y + m_InfoDrawData.Size.y / 2.0f - 0.3f;

    // Define sizes
    static float margin = 0.1f;
    static float scale = 0.2f;

    static glm::vec2 nameTextSize = Renderer2D::GetTextSize(m_Camera, "Name: ", VIEW_FONT) * scale;
    static glm::vec2 nameInfoSize = {
        nameTextSize.x + m_UsernameInputBox->GetSize().x,
        glm::max(nameTextSize.y, m_UsernameInputBox->GetSize().y)
    };

    static float colorInfoQuadScale = 1.4f;
    static glm::vec2 colorTextSize = Renderer2D::GetTextSize(m_Camera, "Color: ", VIEW_FONT) * scale;
    static glm::vec2 colorInfoSize = { colorTextSize + glm::vec2(colorTextSize.y * colorInfoQuadScale) };

    std::string selectedTileEnvName = "none";
    if (m_SelectedTile.TileRef)
        selectedTileEnvName = m_SelectedTile.TileRef->GetEnvironmentName(m_SelectedTile.TileRef->GetEnvironment());

    static glm::vec2 tileTextSize = Renderer2D::GetTextSize(m_Camera, "Tile: ", VIEW_FONT) * scale;
    glm::vec2 tileEnvTextSize = Renderer2D::GetTextSize(m_Camera, selectedTileEnvName, VIEW_FONT) * scale;
    glm::vec2 tileInfoSize = { tileTextSize + tileEnvTextSize };

    // Define positions
    glm::vec2 nameInfoPosition = {
        m_InfoDrawData.Position.x - colorInfoSize.x / 2.0f - nameInfoSize.x / 2.0f - margin,
        addPlayerInfoYPosition
    };

    glm::vec2 colorInfoPosition = {
        m_InfoDrawData.Position.x,
        addPlayerInfoYPosition
    };

    glm::vec2 tileInfoPosition = {
        m_InfoDrawData.Position.x + colorInfoSize.x / 2.0f + tileInfoSize.x / 2.0f + margin,
        addPlayerInfoYPosition
    };

    Renderer2D::DrawTextStr(
        "Name: ",
        {
            nameInfoPosition.x - nameInfoSize.x / 2.0f,
            nameInfoPosition.y
        },
        scale,
        glm::vec3(1.0f),
        HTextAlign::LEFT,
        VTextAlign::MIDDLE,
        VIEW_FONT
    );

    // TODO: Optimize to only update position if window resized
    m_UsernameInputBox->SetPosition({
        nameInfoPosition.x - nameInfoSize.x / 2.0f + nameTextSize.x + m_UsernameInputBox->GetSize().x / 2.0f,
        nameInfoPosition.y
    });
    m_UsernameInputBox->OnUpdate();

    // display color information
    Renderer2D::DrawTextStr(
        "Color: ",
        {
            colorInfoPosition.x - colorInfoSize.x / 2.0f,
            colorInfoPosition.y
        },
        scale,
        glm::vec3(1.0f),
        HTextAlign::LEFT,
        VTextAlign::MIDDLE,
        VIEW_FONT
    );

    Renderer2D::DrawQuad(
        {
            colorInfoPosition.x - colorInfoSize.x / 2.0f + colorTextSize.x + (colorTextSize.y * colorInfoQuadScale) / 2.0f,
            colorInfoPosition.y
        },
        glm::vec2(colorTextSize.y * colorInfoQuadScale),
        glm::vec4(m_NextPlayerInfo.Color, 1.0f)
    );

    // display tile information
    Renderer2D::DrawTextStr(
        "Tile: " + selectedTileEnvName,
        tileInfoPosition,
        scale,
        glm::vec3(1.0f),
        HTextAlign::MIDDLE,
        VTextAlign::MIDDLE,
        VIEW_FONT
    );

    // TODO: Optimize to only update position if window resized
    m_AddPlayerButton->SetPosition(glm::vec2(
        tileInfoPosition.x + tileInfoSize.x / 2.0f + m_AddPlayerButton->GetSize().x / 2.0f + margin,
        tileInfoPosition.y
    ));
    m_AddPlayerButton->OnUpdate();
}

void ChoosePlayersView::DrawPlayerListInfo()
{
    float playerListInfoYPosition = m_InfoDrawData.Position.y;

    Renderer2D::DrawTextStr(
        "Player list",
        { m_InfoDrawData.Position.x, m_InfoDrawData.Position.y + playerListInfoYPosition },
        0.5f,
        glm::vec3(1.0f),
        HTextAlign::MIDDLE,
        VTextAlign::TOP,
        VIEW_FONT
    );

    if (m_PlayersData.empty())
    {
        Renderer2D::DrawTextStr(
            "empty",
            { m_InfoDrawData.Position.x, m_InfoDrawData.Position.y + playerListInfoYPosition - 0.25f},
            0.3f,
            glm::vec3(1.0f),
            HTextAlign::MIDDLE,
            VTextAlign::MIDDLE,
            VIEW_FONT
        );

        return;
    }

    float yPosition = playerListInfoYPosition - 0.2f;
    for (auto player : m_PlayersData)
    {
        std::ostringstream oss;
        oss << "Name: " + player.second.Player.Name + "  Tile: " + player.second.TileEnvName;
        Renderer2D::DrawTextStr(
            oss.str(),
            {
                m_InfoDrawData.Position.x,
                yPosition
            },
            0.25f,
            player.second.Player.Color,
            HTextAlign::MIDDLE,
            VTextAlign::MIDDLE,
            VIEW_FONT
        );

        yPosition -= 0.1f;
    }
}

void ChoosePlayersView::DrawMapPreviewColumn(float dt)
{
    Renderer2D::EndScene();

    m_Framebuffer->Bind();

    Renderer2D::ClearColor({0.0f, 0.0f, 0.0f, 1.0f});

    Renderer2D::BeginScene(m_MapCameraController->GetCamera());

    glm::vec2 relMousePos = m_Camera->CalculateRelativeMousePosition();

    bool mouseOnMap = Util::IsPointInRectangle(m_MapDrawData.Position, m_MapDrawData.Size, relMousePos);
    glm::vec2 mapMousePos;
    if (mouseOnMap && !m_UsernameInputBox->IsFocused())
    {
        m_MapCameraController->OnUpdate(dt);
        mapMousePos = CalculateMousePositionOnMap();
    }

    for (int y = 0; y < m_GameMapManager->GetGameMap()->GetTileCountY(); y++)
    {
        for (int x = 0; x < m_GameMapManager->GetGameMap()->GetTileCountX(); x++)
        {
            auto tile = m_GameMapManager->GetGameMap()->GetTile(x, y);
            bool isCursorOnTile = mouseOnMap && tile->InRange(mapMousePos);
            if (tile->AssetsCanExist())
            {
                glm::vec3 color;
                switch (tile->GetEnvironment())
                {
                    case TileEnvironment::FOREST: color = ColorData::Get().TileColors.ForestColor; break;
                    case TileEnvironment::DESERT: color = ColorData::Get().TileColors.DesertColor; break;
                    case TileEnvironment::MOUNTAINS: color = ColorData::Get().TileColors.MountainsColor; break;
                    default: color = glm::vec3(ColorData::Get().TileColors.MiniMapColor);
                }

                if (isCursorOnTile)
                    color *= 0.7f;

                Renderer2D::DrawHexagon(tile->GetPosition(), glm::vec2(1.0f), glm::vec4(color, 1.0f));

                for (auto it = m_PlayersData.begin(); it != m_PlayersData.end(); it++)
                {
                    if (it->first == tile->GetPosition())
                        Renderer2D::DrawHexagon(tile->GetPosition(), glm::vec2(1.0f), glm::vec4(it->second.Player.Color, 1.0f), 15.0f);
                }

                if (tile == m_SelectedTile.TileRef)
                    Renderer2D::DrawHexagon(tile->GetPosition(), glm::vec2(1.0f), ColorData::Get().TileColors.TileHoverBorderColor, 5.0f);
            }
            else if (tile->GetEnvironment() == TileEnvironment::OCEAN)
            {
                glm::vec3 color = ColorData::Get().TileColors.OceanColor;
                if (isCursorOnTile)
                    color *= 0.7f;
                Renderer2D::DrawHexagon(tile->GetPosition(), glm::vec2(1.0f), glm::vec4(color, 1.0f));
            }
        }
    }

    m_Framebuffer->PostProcess();
    m_Framebuffer->Unbind();

    Renderer2D::EndScene();

    Renderer2D::BeginScene(m_Camera);

    // Draw the map
    Renderer2D::DrawQuad(m_MapDrawData.Position, m_MapDrawData.Size, m_Framebuffer->GetTexture());

    // Draw the border around the map
    Renderer2D::DrawQuad(m_MapDrawData.Position, m_MapDrawData.Size, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), 1.0f);
}

void ChoosePlayersView::OnWindowSizeChanged()
{
    m_StartGameButton->SetPosition(CalculateStartGameButtonPosition());
    m_InfoDrawData = GetColumnDrawData(ColumnType::INFO);
    m_MapDrawData = GetColumnDrawData(ColumnType::MAP);

    auto mapPixelSize = m_Camera->ConvertRelativeSizeToPixel(m_MapDrawData.Size);
    mapPixelSize = Util::Clamp<glm::vec2>(mapPixelSize, glm::vec2(1.0f), mapPixelSize);
    m_Framebuffer = std::make_unique<FrameBuffer>((unsigned int)mapPixelSize.x, (unsigned int)mapPixelSize.y);

    m_MapCameraController->GetCamera()->SetAspectRatio(m_MapDrawData.Size.x / m_MapDrawData.Size.y);

    float zoom = glm::max(m_MapZoom.x / m_MapCameraController->GetCamera()->GetAspectRatio(), m_MapZoom.y);

    m_MapCameraController->GetCamera()->SetZoom(zoom);
    m_MapCameraController->GetCamera()->SetScale(1.0f);
    m_MapCameraController->SetMovementSpeed(zoom);
}

ColumnDrawData ChoosePlayersView::GetColumnDrawData(ColumnType type)
{
    float screenWidth = m_Camera->GetHalfOfRelativeWidth() * 2.0f;
    float screenHeight = m_Camera->GetHalfOfRelativeHeight() * 2.0f;

    glm::vec2 position = glm::vec2(0.0f);
    glm::vec2 size = glm::vec2(screenHeight - (2.0f * m_ColumnData.VMargin));

    if (type == ColumnType::INFO)
    {
        size.x = (screenWidth * m_ColumnData.WidthRatio);
        position.x = (-screenWidth / 2.0f) + (size.x / 2.0f);
    }
    else if (type == ColumnType::MAP)
    {
        size.x = (screenWidth * (1 - m_ColumnData.WidthRatio));
        position.x = (screenWidth / 2.0f) - (size.x / 2.0f);
    }
    else
    {
        LOG_ERROR("GetColumnData: Unknown column type");
    }

    return {
        position,
        glm::max(size, 0.0f)
    };
}

glm::vec2 ChoosePlayersView::CalculateMousePositionOnMap()
{
    auto mapCamera = m_MapCameraController->GetCamera();

    auto minimapBottomLeftPos = m_MapDrawData.Position - m_MapDrawData.Size * 0.5f;
    auto minimapCenter = mapCamera->GetPosition();
    auto m_MapSize = mapCamera->CalculateRelativeWindowSize();

    auto result = m_Camera->CalculateMousePositionInGameObjectInUI(
        minimapBottomLeftPos,
        m_MapDrawData.Size,
        m_MapSize,
        minimapCenter
    );

    return result;
}

glm::vec2 ChoosePlayersView::CalculateStartGameButtonPosition()
{
    const glm::vec2& relWindowSize = MainMenuLayer::Get().GetRelWindowSize();
    return glm::vec2(
        -relWindowSize.x / 2.0f + (m_StartGameButtonSize.x),
        -relWindowSize.y / 2.0f + (m_StartGameButtonSize.y * 2)
    );
}
