#include "console/LuaConsole.h"
#include <iostream>

int main()
{
    console::LuaConsole& console = console::LuaConsole::getInstance();
    std::string test("print(1+2)");
    while(test != "q")
    {
        std::cin>>test;
        console.executeCmd(test);
    }
    return 0;
}
