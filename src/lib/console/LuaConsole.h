#pragma once
#include <lua5.3/lua.hpp>
#include <string>
#include <cstring>  //memset
#include <array>
#include <vector>
#include <imgui/imgui.h>
namespace console {

int LuaError(lua_State* L);
int LuaPrint(lua_State* L);

//! RAII wrapper of lua_state
class LuaState {
public:
    LuaState()
    {
        state_ = luaL_newstate();
        luaL_openlibs(state_);
        lua_getglobal(state_, "_G");
    }
    ~LuaState() { lua_close(state_); }
    void registerFunc(std::string name, lua_CFunction f)
    {
        lua_register(state_, name.c_str(), f);
    }
    int doString(const std::string& str)
    {
        return luaL_dostring(state_, str.c_str());
    }
    std::string getError()
    {
        const char* str = lua_tostring(state_, -1);
        lua_pop(state_, 1);
        return std::string(str);
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
    void executeCmd(const std::string& cmd);
    void draw();

private:
    LuaConsole()
    {
        clearLog_();
        inputBuffer_.fill(0);
        historyPos_ = -1;
        state_.registerFunc(" ALERT", LuaError);
        state_.registerFunc("print", LuaPrint);
        addLog_("Started");
    }
    ~LuaConsole()
    {
        clearLog_();
    }
    LuaState state_;
    std::array<char, 256> inputBuffer_;
    std::vector<std::string> items_;
    bool scrollToBottom_;
    std::vector<std::string> history_;   //!< -1: new line, 0..History.Size-1 browsing history.
    int historyPos_;
    ImVector<const char*> commands_;
    int TextEditCallback(ImGuiTextEditCallbackData* data);

    void clearLog_();
    void addLog_(std::string);
};
}  // namespace console
