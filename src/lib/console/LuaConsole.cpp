#include "LuaConsole.h"
#include <iostream>
#include <sstream>
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
    lua_pop(L, 1);
    int nArgs = lua_gettop(L);
    std::stringstream ss;
    for (int i=0; i<=nArgs; i++)
    {
        ss<<lua_tostring(L, i);
    }
    LuaConsole::getInstance().putStr(ss.str());
    return 0;
};

void LuaConsole::putStr(const std::string& str) {
    std::cout<<str<<std::endl;
}
void LuaConsole::executeCmd(const std::string& cmd)
{
    state_.doString(cmd);
}

// UI 
void LuaConsole::clearLog_()
{
    items_.clear();
    scrollToBottom_ = true;
}

void LuaConsole::addLog_(std::string log)
{
    items_.push_back(log);
    scrollToBottom_ = true;
}

void LuaConsole::draw()
{
    ImGui::Begin("Lua Console");
    ImGui::BeginChild("ScrollingRegion",
                      ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false,
                      ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::Selectable("Clear")) clearLog_();
        ImGui::EndPopup();
    }

    // Display every line as a separate entry so we can change their color or
    // add custom widgets. If you only want raw text you can use
    // ImGui::TextUnformatted(log.begin(), log.end());
    // NB- if you have thousands of entries this approach may be too inefficient
    // and may require user-side clipping to only process visible items.
    // You can seek and display only the lines that are visible using the
    // ImGuiListClipper helper, if your elements are evenly spaced and you have
    // cheap random access to the elements.
    // To use the clipper we could replace the 'for (int i = 0; i < Items.Size;
    // i++)' loop with:
    //     ImGuiListClipper clipper(Items.Size);
    //     while (clipper.Step())
    //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
    // However take note that you can not use this code as is if a filter is
    // active because it breaks the 'cheap random-access' property. We would
    // need random-access on the post-filtered list.
    // A typical application wanting coarse clipping and filtering may want to
    // pre-compute an array of indices that passed the filtering test,
    // recomputing this array when user changes the filter,
    // and appending newly elements as they are inserted. This is left as a task
    // to the user until we can manage to improve this example code!
    // If your items are of variable size you may want to implement code similar
    // to what ImGuiListClipper does. Or split your data into fixed height items
    // to allow random-seeking into your list.
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                        ImVec2(4, 1));  // Tighten spacing
    // if (copy_to_clipboard) ImGui::LogToClipboard();
    for (const auto& item : items_) {
        // if (!filter.PassFilter(item)) continue;
        ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // A better implementation
                                                      // may store a type
                                                      // per-item. For the
                                                      // sample let's just parse
                                                      // the text.
        if (item == "[error]")
            col = ImColor(1.0f, 0.4f, 0.4f, 1.0f);
        else if (item[2] == 0)
            col = ImColor(1.0f, 0.78f, 0.58f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::TextUnformatted(item.c_str());
        ImGui::PopStyleColor();
    }
    // if (copy_to_clipboard) ImGui::LogFinish();
    if (scrollToBottom_) ImGui::SetScrollHere();
    scrollToBottom_ = false;
    ImGui::PopStyleVar();
    ImGui::EndChild();

    ImGui::End();
}
}  // namespace consol
