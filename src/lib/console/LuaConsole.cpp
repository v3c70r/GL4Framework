#include "LuaConsole.h"
#include <iostream>
#include <sstream>
namespace console {
int LuaError(lua_State* L)
{
    const char* str = lua_tostring(L, -1);
    lua_pop(L, 1);
    // send string wherever in my case the console
    std::cout<<"alert\n";
    std::cout<<str;
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
    addLog_(str);
}
void LuaConsole::executeCmd(const std::string& cmd)
{
    if (state_.doString(cmd))
        putStr(state_.getError());
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

    ImGui::Separator();
    // Command-line
    if (ImGui::InputText("Input", inputBuffer_.data(), inputBuffer_.size(),
                         ImGuiInputTextFlags_EnterReturnsTrue)) {
        // Remove taling spaces
        //char* input_end = inputBuffer_.end();
        //while (input_end > inputBuffer_.begin() && input_end[-1] == ' ') {
        //    input_end--;
        //}
//        *input_end = 0;
        putStr(">>> " + std::string(inputBuffer_.data()));
        if (inputBuffer_[0]) executeCmd(std::string(inputBuffer_.data()));
        inputBuffer_.fill(0);
    }

    // Demonstrate keeping auto focus on the input box
     if (ImGui::IsItemHovered() ||
        (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive()
        &&
         !ImGui::IsMouseClicked(0)))
        ImGui::SetKeyboardFocusHere(-1);  // Auto focus previous widget

    ImGui::End();
}
//int     LuaConsole::TextEditCallback(ImGuiTextEditCallbackData* data)
//    {
//        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
//        switch (data->EventFlag)
//        {
//        case ImGuiInputTextFlags_CallbackCompletion:
//            {
//                // Example of TEXT COMPLETION
//
//                // Locate beginning of current word
//                const char* word_end = data->Buf + data->CursorPos;
//                const char* word_start = word_end;
//                while (word_start > data->Buf)
//                {
//                    const char c = word_start[-1];
//                    if (c == ' ' || c == '\t' || c == ',' || c == ';')
//                        break;
//                    word_start--;
//                }
//
//                // Build a list of candidates
//                ImVector<const char*> candidates;
//                for (int i = 0; i < Commands.Size; i++)
//                    if (Strnicmp(Commands[i], word_start, (int)(word_end-word_start)) == 0)
//                        candidates.push_back(Commands[i]);
//
//                if (candidates.Size == 0)
//                {
//                    // No match
//                    addLog_("No match for \"%.*s\"!\n", (int)(word_end-word_start), word_start);
//                }
//                else if (candidates.Size == 1)
//                {
//                    // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
//                    data->DeleteChars((int)(word_start-data->Buf), (int)(word_end-word_start));
//                    data->InsertChars(data->CursorPos, candidates[0]);
//                    data->InsertChars(data->CursorPos, " ");
//                }
//                else
//                {
//                    // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
//                    int match_len = (int)(word_end - word_start);
//                    for (;;)
//                    {
//                        int c = 0;
//                        bool all_candidates_matches = true;
//                        for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
//                            if (i == 0)
//                                c = toupper(candidates[i][match_len]);
//                            else if (c == 0 || c != toupper(candidates[i][match_len]))
//                                all_candidates_matches = false;
//                        if (!all_candidates_matches)
//                            break;
//                        match_len++;
//                    }
//
//                    if (match_len > 0)
//                    {
//                        data->DeleteChars((int)(word_start - data->Buf), (int)(word_end-word_start));
//                        data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
//                    }
//
//                    // List matches
//                    addLog_("Possible matches:\n");
//                    for (int i = 0; i < candidates.Size; i++)
//                        addLog_("- %s\n", candidates[i]);
//                }
//
//                break;
//            }
//        case ImGuiInputTextFlags_CallbackHistory:
//            {
//                // Example of HISTORY
//                const int prev_history_pos = HistoryPos;
//                if (data->EventKey == ImGuiKey_UpArrow)
//                {
//                    if (HistoryPos == -1)
//                        HistoryPos = History.Size - 1;
//                    else if (HistoryPos > 0)
//                        HistoryPos--;
//                }
//                else if (data->EventKey == ImGuiKey_DownArrow)
//                {
//                    if (HistoryPos != -1)
//                        if (++HistoryPos >= History.Size)
//                            HistoryPos = -1;
//                }
//
//                // A better implementation would preserve the data on the current input line along with cursor position.
//                if (prev_history_pos != HistoryPos)
//                {
//                    data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen = (int)snprintf(data->Buf, (size_t)data->BufSize, "%s", (HistoryPos >= 0) ? History[HistoryPos] : "");
//                    data->BufDirty = true;
//                }
//            }
//        }
//        return 0;
//    }
}  // namespace consol
