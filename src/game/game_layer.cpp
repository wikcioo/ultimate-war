#include "game_layer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game/tile.h"
#include "debug/debug_data.h"
#include "core/core.h"
#include "core/input.h"
#include "core/logger.h"
#include "core/resource_manager.h"
#include "graphics/renderer.h"
#include "util/util.h"

GameLayer* GameLayer::s_Instance = nullptr;

GameLayer::GameLayer()
    : Layer("GameLayer"), m_IterationNumber(0), m_GameActive(true)
{
    s_Instance = this;

    auto window = Application::Get().GetWindow();
    m_CameraController = std::make_shared<OrthographicCameraController>((float)window->GetWidth() / (float)window->GetHeight(), true);
    m_GameMapManager = std::make_shared<GameMapManager>("");
    m_PlayerManager = std::make_shared<PlayerManager>();
    m_Arrow = std::make_shared<Arrow>();

    m_PlayerManager->AddPlayer(Util::GenerateAnonymousName(), {1.0f, 0.0f, 0.0f});
    m_PlayerManager->AddPlayer(Util::GenerateAnonymousName(), {0.0f, 0.0, 1.0f});
}

void GameLayer::OnAttach()
{
    m_GameMapManager->Load("simple");

#if defined(DEBUG)
    int i = 2;
    for (auto player : m_PlayerManager->GetAllPlayers())
    {
        auto tile = m_GameMapManager->GetGameMap()->GetTile(i-1, i+1);
        tile->CreateUnit(UnitType::SWORDSMAN);
        tile->CreateUnit(UnitType::DWARF);
        tile->CreateUnit(UnitType::DEMON);
        tile->CreateBuilding(BuildingType::DRAGON_LAIR);
        player->AddOwnedTile(tile);
        i++;
    }
#endif
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(float dt)
{
    m_CameraController->OnUpdate(dt);

    Renderer2D::ClearColor({0.0f, 0.5f, 1.0f, 1.0f});

    Renderer2D::BeginScene(m_CameraController->GetCamera());

    auto relMousePos = m_CameraController->GetCamera()->CalculateRelativeMousePosition();
    bool isCursorInRange = false;
    for (int y = 0; y < m_GameMapManager->GetGameMap()->GetTileCountY(); y++)
    {
        for (int x = 0; x < m_GameMapManager->GetGameMap()->GetTileCountX(); x++)
        {
            auto tile = m_GameMapManager->GetGameMap()->GetTile(x, y);

            glm::vec4 tileColor;
            if (!isCursorInRange && tile->InRange(relMousePos))
            {
                auto startTile = m_Arrow->GetStartTile();
                if (startTile && Tile::IsAdjacent({x, y}, startTile->GetCoords()) && tile->GetType() != 0)
                {
                    isCursorInRange = true;
                    m_Arrow->SetEndPosition(tile->GetPosition());
                }

                tileColor = ColorData::Get().TileColors.TileHoverBorderColor;
            }
            else
            {
                if (tile->GetType() == 0)
                    tileColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.1f);
                else
                    tileColor = glm::vec4(1.0f);
            }

            tile->Draw(tileColor);
        }
    }

    if (m_Arrow->IsVisible() && !isCursorInRange)
        m_Arrow->SetEndPosition(m_Arrow->GetStartTile()->GetPosition());

    m_Arrow->Draw();

    Renderer2D::EndScene();
}

void GameLayer::OnEvent(Event& event)
{
    m_CameraController->OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(GameLayer::OnMouseButtonPressed));
    dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(GameLayer::OnKeyReleased));
}

bool GameLayer::OnKeyReleased(KeyReleasedEvent& event)
{
    if(!m_GameActive) return true;

    if(event.GetKeyCode() == GLFW_KEY_ENTER)
    {
        m_PlayerManager->NextTurn();
        return true;
    }

    return false;
}

void GameLayer::ResetArrow()
{
    for(auto tile : m_PlayerManager->GetCurrentPlayer()->GetOwnedTiles())
    {
        tile->DeselectAllUnits();
    }
    m_Arrow->SetVisible(false);
}

void GameLayer::EndGame()
{
    m_GameActive = false;
}

bool GameLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event)
{
    if(!m_GameActive) return true;

    auto relMousePos = m_CameraController->GetCamera()->CalculateRelativeMousePosition();
    auto currentPlayer = m_PlayerManager->GetCurrentPlayer();

    for (int y = 0; y < m_GameMapManager->GetGameMap()->GetTileCountY(); y++)
    {
        for (int x = 0; x < m_GameMapManager->GetGameMap()->GetTileCountX(); x++)
        {
            auto tile = m_GameMapManager->GetGameMap()->GetTile(x, y);
            if (tile->InRange(relMousePos))
            {
                if (!m_Arrow->GetStartTile()) m_Arrow->SetStartTile(tile);

                if (m_Arrow->GetStartTile() != tile)
                {
                    if (tile->GetType() != 0)
                    {
                        if (m_Arrow->GetStartTile()->HasSelectedUnits())
                        {
                            if (Tile::IsAdjacent(m_Arrow->GetStartTile()->GetCoords(), tile->GetCoords()))
                            {
                                m_Arrow->GetStartTile()->MoveToTile(tile);
                            }
                            else
                            {
                                m_Arrow->GetStartTile()->DeselectAllUnits();
                            }
                        }
                        else if (tile->GetOwnedBy() == currentPlayer)
                        {
                            m_Arrow->SetStartTile(tile);
                            tile->HandleUnitMouseClick(relMousePos);
                        }
                    }
                    else
                    {
                        m_Arrow->GetStartTile()->DeselectAllUnits();
                    }
                }
                else if (tile->GetOwnedBy() == currentPlayer)
                {
                    if (!tile->HandleUnitMouseClick(relMousePos))
                    {
                        if (!tile->IsMouseClickedInsideUnitsBox(relMousePos))
                        {
                            tile->DeselectAllUnits();
                        }
                    }
                }

                goto tile_in_range;
            }
        }
    }

    if (m_Arrow->GetStartTile())
        m_Arrow->GetStartTile()->DeselectAllUnits();

    tile_in_range:
    m_Arrow->SetVisible(m_Arrow->GetStartTile() && m_Arrow->GetStartTile()->HasSelectedUnits());

    return true;
}
