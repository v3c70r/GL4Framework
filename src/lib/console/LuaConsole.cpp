#include "LuaConsole.h"
#include "core/log.hpp"
namespace console {
int LuaError(lua_State* L)
{
    const char* str = lua_tostring(L, -1);
    lua_pop(L, 1);
    // send string wherever in my case the console
    LuaConsole::getInstance().putStr(std::string(str));
    return 0;
}
int LuaPrint(lua_State *L)
{
    int nArgs = lua_gettop(L);
    int i;
    lua_getglobal(L, "tostring");
    std::string ret;//this is where we will dump the output
    //make sure you start at 1 *NOT* 0
    for(i = 1; i <= nArgs; i++)
    {
        const char *s;
        lua_pushvalue(L, -1);
        lua_pushvalue(L, i);
        lua_call(L, 1, 1);
        s = lua_tostring(L, -1);
        if(s == NULL)
            return luaL_error(L, LUA_QL("tostring") " must return a string to ", LUA_QL("print"));
 
        if(i > 1) ret.append("\t");
 
        ret.append(s);
        lua_pop(L, 1);
    };
    ret.append("\n");
    //Send it wherever
    LuaConsole::getInstance().putStr(ret.c_str());
    return 0;
};

void LuaConsole::putStr(const std::string& str) {
    LOG::writeLog( "[LUA] %s\n", str.c_str());
}
}  // namespace console
