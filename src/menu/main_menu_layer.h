#pragma once

#include <memory>
#include <unordered_map>

#include "core/camera.h"
#include "layer/layer.h"
#include "event/event.h"
#include "event/window_event.h"
#include "widgets/button.h"
#include "graphics/texture.h"

enum class ViewName
{
    MAIN,
    CHOOSE_MAP
};

class MainMenuView;

class MainMenuLayer : public Layer
{
public:
    MainMenuLayer();
    ~MainMenuLayer();

    static MainMenuLayer& Get() { return *s_Instance; }

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnEvent(Event& event) override;

    void SetView(ViewName viewName);
    const glm::vec2& GetRelWindowSize() { return m_RelWindowSize; }
    const std::shared_ptr<OrthographicCamera>& GetMainMenuCamera() { return m_MainMenuCamera; }

    const std::string& GetSelectedMap();

private:
    void OnWindowSizeChanged();

private:
    static MainMenuLayer* s_Instance;

    glm::vec2 m_RelWindowSize;
    ViewName m_CurrentViewName;
    std::shared_ptr<Texture2D> m_Background;
    std::unordered_map<ViewName, MainMenuView*> m_Views;
    std::shared_ptr<OrthographicCamera> m_MainMenuCamera;
};
