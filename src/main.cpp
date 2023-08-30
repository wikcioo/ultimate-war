#include "core/application.h"

int main(int argc, char* argv[])
{
    std::unique_ptr<Application> app = std::make_unique<Application>();
    app->Run();

    return 0;
}
