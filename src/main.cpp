#include "core/logger.h"

int main(int argc, char* argv[])
{
    Logger::Init();
    LOG_INFO("Hello, {0}", "World!");

    return 0;
}
