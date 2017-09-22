#pragma once
#include <lua5.3/lua.hpp>
#include <string>

namespace console {

int LuaError(lua_State* L);
int LuaPrint(lua_State *L);

//! RAII wrapper of lua_state
class LuaState {
public:
    LuaState() { state_ = luaL_newstate(); }
    ~LuaState() { lua_close(state_); }
    void registerFunc(
            std::string name,
            lua_CFunction f
            )
    {
        lua_register(state_, name.c_str(), f);
    }
private:
    lua_State* state_;
};
class LuaConsole {
public:
    static LuaConsole& getInstance()
    {
        static LuaConsole instance;
        return instance;
    }
    LuaConsole(LuaConsole const&) = delete;
    void operator=(LuaConsole const&) = delete;
    void putStr(const std::string& str);

private:
    LuaConsole() {
        state_.registerFunc(" ALERT", LuaError);
        state_.registerFunc(" print", LuaPrint);
    }
    LuaState state_;
};
}  // namespace console
