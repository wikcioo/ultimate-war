#include "choose_save_view.h"

#include "util/util.h"
#include "graphics/renderer.h"
#include "core/application.h"
#include "loader/save_loader.h"

ChooseSaveView::ChooseSaveView()
    : BackableView(ViewName::MAIN)
{
}

ChooseSaveView::~ChooseSaveView()
{
    for (auto button : m_Buttons)
        delete button;
}

void ChooseSaveView::OnAttach()
{
    BackableView::OnAttach();
    m_Buttons.clear();

    auto saves = SaveLoader::GetAvailableSaves();

    static float yOffset = 0.05f;
    static float buttonHeight = 0.1f;
    float totalHeight = (saves.size() * buttonHeight) + ((saves.size() - 1) * yOffset);
    float yStartPosition = totalHeight / 2.0f - buttonHeight / 2.0f;

    for (const auto& save : saves)
    {
        Button* saveButton = new Button(
            m_Camera,
            { Util::StrToUpper(save), glm::vec2(0.0f, yStartPosition), glm::vec2(1.0f, 0.1f) }
        );
        saveButton->SetPressedCallback(BIND_BTN_CALLBACK_FN(ChooseSaveView::OnSaveButtonPressed));
        m_Buttons.emplace_back(saveButton);

        yStartPosition -= buttonHeight + yOffset;
    }
}

void ChooseSaveView::OnUpdate(float dt)
{
    Renderer2D::DrawTextStr(
        "Choose the save",
        { 0.0f, m_Camera->GetHalfOfRelativeHeight() - 0.1f },
        0.7f,
        glm::vec3(1.0f),
        HTextAlign::MIDDLE,
        VTextAlign::TOP,
        "rexlia"
    );

    for (auto button : m_Buttons)
        button->OnUpdate();

    BackableView::OnUpdate(dt);
}

void ChooseSaveView::OnEvent(Event& event)
{
    for (auto button : m_Buttons)
        button->OnEvent(event);

    BackableView::OnEvent(event);
}

void ChooseSaveView::OnSaveButtonPressed(ButtonCallbackData data)
{
    Application::Get().LoadSave(Util::StrToLower(data.Text));
}
