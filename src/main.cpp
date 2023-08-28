#include "core/application.h"

#include "event/event.h"
#include "event/mouse_event.h"

int main(int argc, char* argv[])
{
    std::unique_ptr<Application> app = std::make_unique<Application>();
    app->Run();

    auto event = MouseMovedEvent(100, 200);
    LOG_TRACE("{0}", event.ToString());

    return 0;
}
