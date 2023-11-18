#include "main_menu_layer.h"

#include "core/application.h"
#include "graphics/renderer.h"
#include "menu/views/main_view.h"
#include "menu/views/choose_map_view.h"
#include "menu/views/contributors_view.h"

MainMenuLayer* MainMenuLayer::s_Instance = nullptr;

MainMenuLayer::MainMenuLayer()
    : Layer("MainMenuLayer")
{
    s_Instance = this;

    auto window = Application::Get().GetWindow();
    float aspectRatio = (float)window->GetWidth() / (float)window->GetHeight();
    m_MainMenuCamera = std::make_shared<OrthographicCamera>(aspectRatio);
    m_RelWindowSize = m_MainMenuCamera->CalculateRelativeWindowSize();

    InitViews();
}

MainMenuLayer::~MainMenuLayer()
{
    for (auto it = m_Views.begin(); it != m_Views.end(); it++)
        delete it->second;
}

void MainMenuLayer::OnAttach()
{
    RecalculateCamera();
    SetView(ViewName::MAIN);
}

void MainMenuLayer::OnDetach()
{
}

void MainMenuLayer::OnUpdate(float dt)
{
    Renderer2D::BeginScene(m_MainMenuCamera);

    Renderer2D::DrawQuad(glm::vec2(0.0f), m_RelWindowSize, glm::vec4(0.2f, 0.5f, 0.8f, 1.0f));

    m_Views[m_CurrentViewName]->OnUpdate(dt);

    Renderer2D::EndScene();
}

void MainMenuLayer::OnEvent(Event& event)
{
    if (event.GetCategory() == EventCategory::Window)
        OnWindowSizeChanged();

    m_Views[m_CurrentViewName]->OnEvent(event);
}

void MainMenuLayer::SetView(ViewName viewName)
{
    m_Views[viewName]->OnAttach();
    m_CurrentViewName = viewName;
}

const std::string& MainMenuLayer::GetSelectedMap()
{
    return ((ChooseMapView*)m_Views[ViewName::CHOOSE_MAP])->GetSelectedMap();
}

void MainMenuLayer::InitViews()
{
    m_Views[ViewName::MAIN] = new MainView();
    m_Views[ViewName::MAIN]->OnAttach();
    m_Views[ViewName::CHOOSE_MAP] = new ChooseMapView();
    m_Views[ViewName::CHOOSE_MAP]->OnAttach();
    m_Views[ViewName::CONTRIBUTORS] = new ContributorsView();
    m_Views[ViewName::CONTRIBUTORS]->OnAttach();

    m_CurrentViewName = ViewName::MAIN;
}

void MainMenuLayer::OnWindowSizeChanged()
{
    RecalculateCamera();
}

void MainMenuLayer::RecalculateCamera()
{
    static auto window = Application::Get().GetWindow();
    m_MainMenuCamera->SetWindowAspectRatio();
    m_MainMenuCamera->SetScale(window->GetHeight() / INITIAL_RELATIVE_HEIGHT_IN_PIXELS);
    m_RelWindowSize = m_MainMenuCamera->CalculateRelativeWindowSize();
}
