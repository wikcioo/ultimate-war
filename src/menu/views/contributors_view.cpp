#include "contributors_view.h"

#include "graphics/renderer.h"

ContributorsView::ContributorsView()
    : BackableView(ViewName::MAIN)
{
}

void ContributorsView::OnUpdate(float dt)
{
    Renderer2D::DrawTextStr(
        "CONTRIBUTORS",
        glm::vec2(0.0f, 0.4f),
        0.8f,
        glm::vec3(1.0f),
        HTextAlign::MIDDLE,
        VTextAlign::MIDDLE,
        "rexlia"
    );

    DrawContributors();

    BackableView::OnUpdate(dt);
}

void ContributorsView::OnEvent(Event& event)
{
    BackableView::OnEvent(event);
}

void ContributorsView::DrawContributors()
{
    Renderer2D::DrawTextStr(
        "Aivaras Zevzikovas",
        glm::vec2(0.0f, 0.0f),
        0.4f,
        glm::vec3(1.0f),
        HTextAlign::MIDDLE,
        VTextAlign::MIDDLE,
        "rexlia"
    );

    Renderer2D::DrawTextStr(
        "Wiktor Ciolkowski",
        glm::vec2(0.0f, -0.2f),
        0.4f,
        glm::vec3(1.0f),
        HTextAlign::MIDDLE,
        VTextAlign::MIDDLE,
        "rexlia"
    );
}
