#pragma once

#include <memory>
#include <vector>

#include "ui/ui_element.h"

class UIElementStack
{
public:
    UIElementStack() = default;
    ~UIElementStack() = default;

    void PushElement(std::shared_ptr<UIElement> element)
    {
        m_Elements.emplace_back(element);
    }

    void PopElement(std::shared_ptr<UIElement> element)
    {
        auto it = std::find(m_Elements.begin(), m_Elements.end(), element);
        if (it != m_Elements.end())
        {
            m_Elements.erase(it);
        }
    }

    void Clear()
    {
        m_Elements.clear();
    }

    std::vector<std::shared_ptr<UIElement>>::iterator begin() { return m_Elements.begin(); }
    std::vector<std::shared_ptr<UIElement>>::iterator end() { return m_Elements.end(); }
    std::vector<std::shared_ptr<UIElement>>::reverse_iterator rbegin() { return m_Elements.rbegin(); }
    std::vector<std::shared_ptr<UIElement>>::reverse_iterator rend() { return m_Elements.rend(); }

private:
    std::vector<std::shared_ptr<UIElement>> m_Elements;
};
