#if defined(DEBUG)

#pragma once

#include "game/game_layer.h"
#include "layer/layer.h"

class DebugLayer : public Layer
{
public:
    DebugLayer();
    ~DebugLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(Event& event) override;
    virtual void OnUpdate(float dt) override;

private:
    void BeginFrame();
    void EndFrame();
    void DisplayInfoWindow(float dt);
    void DisplaySettingsWindow();
    void DisplayPlayerWindow();

private:
    GameLayer& m_GameLayer;
};

#endif
