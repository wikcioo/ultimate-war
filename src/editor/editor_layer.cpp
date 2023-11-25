#include "editor/editor_layer.h"

#include <fstream>

#include "core/application.h"
#include "core/input.h"
#include "graphics/renderer.h"

EditorLayer* EditorLayer::s_Instance = nullptr;

EditorLayer::EditorLayer()
    : Layer("EditorLayer")
{
    s_Instance = this;
    auto window = Application::Get().GetWindow();
    m_CameraController = std::make_shared<OrthographicCameraController>((float)window->GetWidth() / (float)window->GetHeight(), true);
}

void EditorLayer::OnAttach()
{
    auto coords = glm::ivec2(0);
    Tile* baseTile = new Tile(TileEnvironment::HIGHLIGHT, coords);
    m_PreviousTile = baseTile;
    m_Map.insert({ coords, baseTile });
    m_SelectedTileEnvType = TileEnvironment::FOREST;

    m_CameraController->GetCamera()->SetPosition({ baseTile->GetPosition(), 0.0f });
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(float dt)
{
    if (m_CameraController->IsKeyPressed(GLFW_KEY_W) ||
        m_CameraController->IsKeyPressed(GLFW_KEY_S) ||
        m_CameraController->IsKeyPressed(GLFW_KEY_A) ||
        m_CameraController->IsKeyPressed(GLFW_KEY_D))
    {
        if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
            CheckForTileInRange();
    }
    m_CameraController->OnUpdate(dt);

    Renderer2D::ClearColor({0.2f, 0.2f, 0.2f, 1.0f});

    Renderer2D::BeginScene(m_CameraController->GetCamera());

    for (const auto& pair : m_Map) {
        pair.second->Draw();
    }

    Renderer2D::EndScene();
}

void EditorLayer::CreateAdjacentHightlightTiles(glm::ivec2 coords)
{
    int offset = 0;
    if (coords.x % 2 == 0)
        offset = -1;

    for (auto tileOffset : Tile::s_AdjacentTileOffsets)
    {
        glm::ivec2 location;
        if (tileOffset.x != 0)
        {
            location = glm::ivec2(coords.x + tileOffset.x, coords.y + offset + tileOffset.y);
        }
        else
        {
            location = glm::ivec2(coords.x + tileOffset.x, coords.y + tileOffset.y);
        }

        if (m_Map.find(location) == m_Map.end())
        {
            Tile* border = new Tile(TileEnvironment::HIGHLIGHT, location);
            m_Map.insert({ location, border });
        }
    }
}

void EditorLayer::OnEvent(Event& event)
{
    m_CameraController->OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseMoved));
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
}

void EditorLayer::SaveMap(const std::string& mapName)
{
    int minX = 0, minY = 0;
    int maxX = 0, maxY = 0;

    for (const auto& pair : m_Map) {
        auto coords = pair.first;
        if (pair.second->GetEnvironment() == TileEnvironment::HIGHLIGHT)
            continue;

        if (coords.x < minX)
        {
            minX = coords.x;
        }
        if (coords.y < minY)
        {
            minY = coords.y;
        }
        if (coords.x > maxX)
        {
            maxX = coords.x;
        }
        if (coords.y > maxY)
        {
            maxY = coords.y;
        }
    }

    std::ofstream outputFile("assets/maps/" + mapName + ".map");
    bool addExtraLine = minX % 2 != 0;
    if (outputFile.is_open())
    {
        for (int y = maxY; y >= minY; y--)
        {
            if (addExtraLine){
                outputFile << 0 << " ";
            }
            for (int x = minX; x <= maxX; x++)
            {
                auto it = m_Map.find({x, y});
                if (it != m_Map.end() && it->second->GetEnvironment() != TileEnvironment::HIGHLIGHT)
                {
                    outputFile << (int)it->second->GetEnvironment() << " ";
                }
                else
                {
                    outputFile << 1 << " ";
                }
            }
            outputFile << '\n';
        }

        outputFile.close();
    }
    else
    {
        LOG_ERROR("EditorLayer: Unable to save the file");
    }
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
{
    switch (event.GetKeyCode())
    {
        case GLFW_KEY_TAB:
        {
            if (Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
                Application::Get().OpenMainMenu();
            break;
        }
        case GLFW_KEY_1:
        {
            m_SelectedTileEnvType = TileEnvironment::FOREST;
            break;
        }
        case GLFW_KEY_2:
        {
            m_SelectedTileEnvType = TileEnvironment::DESERT;
            break;
        }
        case GLFW_KEY_3:
        {
            m_SelectedTileEnvType = TileEnvironment::MOUNTAINS;
            break;
        }
        case GLFW_KEY_4:
        {
            m_SelectedTileEnvType = TileEnvironment::OCEAN;
            break;
        }
        case GLFW_KEY_5:
        {
            m_SelectedTileEnvType = TileEnvironment::NONE;
            break;
        }
        default:
            break;
    }

    return false;
}

bool EditorLayer::OnMouseMoved(MouseMovedEvent& event)
{
    if (!Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
        return true;

    CheckForTileInRange();
    return true;
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event)
{
    CheckForTileInRange();
    return true;
}

// Remove a tile if it has no adjacent environment tile
void EditorLayer::RemoveTileWithNoAdjacent(Tile* tile)
{
    if (tile->GetEnvironment() != TileEnvironment::HIGHLIGHT)
        return;

    int offset = 0;

    auto coords = tile->GetCoords();

    if (coords.x % 2 == 0)
        offset = -1;

    for (auto tileOffset : Tile::s_AdjacentTileOffsets)
    {
        glm::ivec2 location;
        if (tileOffset.x != 0)
        {
            location = glm::ivec2(coords.x + tileOffset.x, coords.y + offset + tileOffset.y);
        }
        else
        {
            location = glm::ivec2(coords.x + tileOffset.x, coords.y + tileOffset.y);
        }

        auto it = m_Map.find(location);
        if (it != m_Map.end() && it->second->GetEnvironment() != TileEnvironment::HIGHLIGHT)
        {
            return;
        }
    }

    if (m_Map.size() > 1)
    {
        auto it = m_Map.find(coords);
        if (it != m_Map.end())
        {
            m_Map.erase(it);
        }
    }
}

void EditorLayer::CheckAdjacentTilesForRemoval(Tile* tile)
{
    int offset = 0;
    auto coords = tile->GetCoords();

    if (coords.x % 2 == 0)
        offset = -1;

    for (auto tileOffset : Tile::s_AdjacentTileOffsets)
    {
        glm::ivec2 location;
        if (tileOffset.x != 0)
        {
            location = glm::ivec2(coords.x + tileOffset.x, coords.y + offset + tileOffset.y);
        }
        else
        {
            location = glm::ivec2(coords.x + tileOffset.x, coords.y + tileOffset.y);
        }

        auto it = m_Map.find(location);
        if (it != m_Map.end())
        {
            RemoveTileWithNoAdjacent(it->second);
        }
    }
}

void EditorLayer::UpdateTile(Tile* tile)
{
    if (m_SelectedTileEnvType == TileEnvironment::NONE)
    {
        tile->SetEnvironment(TileEnvironment::HIGHLIGHT);
        CheckAdjacentTilesForRemoval(tile);
        RemoveTileWithNoAdjacent(tile);
        return;
    }

    tile->SetEnvironment(m_SelectedTileEnvType);
    CreateAdjacentHightlightTiles(tile->GetCoords());
    m_PreviousTile = tile;
}

void EditorLayer::CheckForTileInRange()
{
    auto relMousePos = m_CameraController->GetCamera()->CalculateRelativeMousePosition();

    // Check it tile is the same as previously clicked to optimize for holding left click
    if (m_PreviousTile->InRange(relMousePos))
    {
        UpdateTile(m_PreviousTile);
        return;
    }

    for (const auto& pair : m_Map) {
        auto tile = pair.second;

        if (!tile->InRange(relMousePos))
            continue;

        UpdateTile(tile);
        return;
    }
}
