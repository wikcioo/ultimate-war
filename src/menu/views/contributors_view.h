#pragma once

#include "widgets/button.h"
#include "menu/views/backable_view.h"

class ContributorsView : public BackableView
{
public:
    ContributorsView();
    ~ContributorsView() = default;

    virtual void OnUpdate() override;
    virtual void OnEvent(Event& event) override;

private:
    void DrawContributors();
};
