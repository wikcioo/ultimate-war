#pragma once

#include "layer/layer.h"

class DebugLayer : public Layer
{
public:
    DebugLayer(const std::string& name = "Debug Layer");
    ~DebugLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(Event& event) override;

    void BeginFrame();
    void EndFrame();
};
