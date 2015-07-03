#include "App.hpp"

int main()
{
    App application;
    App::init();
    App::run();
    App::deactivate();
    return 0;
}

