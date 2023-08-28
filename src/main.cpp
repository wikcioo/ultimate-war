#include "core/logger.h"

#include "event/event.h"
#include "event/mouse_event.h"

int main(int argc, char* argv[])
{
    Logger::Init();
    LOG_INFO("Hello, {0}", "World!");

    auto event = MouseMovedEvent(100, 200);
    LOG_TRACE("{0}", event.ToString());

    return 0;
}
