#pragma once

#include <memory>
#include <vector>

#include "layer.h"

class LayerStack
{
public:
    LayerStack() = default;
    ~LayerStack();

    void PushLayer(std::shared_ptr<Layer> layer);
    void PopLayer(std::shared_ptr<Layer> layer);

    void PushOverlay(std::shared_ptr<Layer> overlay);
    void PopOverlay(std::shared_ptr<Layer> overlay);

    void Enable(std::shared_ptr<Layer> layer) { layer->SetIsActive(true); }
    void Disable(std::shared_ptr<Layer> layer) { layer->SetIsActive(false); }

    std::vector<std::shared_ptr<Layer>>::iterator begin() { return m_Layers.begin(); }
    std::vector<std::shared_ptr<Layer>>::iterator end() { return m_Layers.end(); }
    std::vector<std::shared_ptr<Layer>>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
    std::vector<std::shared_ptr<Layer>>::reverse_iterator rend() { return m_Layers.rend(); }

private:
    std::vector<std::shared_ptr<Layer>> m_Layers;
    unsigned int m_LayerInsertIndex = 0;
};
