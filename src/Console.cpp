// Console.cpp
// Class to manage IMGUI console for debugging

#include "Console.h"

// Avoid cyclic dependencies
#include "Game.h"

// Constructor
Console::Console() {
  clearLog();
  memset(InputBuf, 0, sizeof(InputBuf));
  historyPos_ = -1;
  commands_.push_back("HELP");
  commands_.push_back("HISTORY");
  commands_.push_back("CLEAR");
  commands_.push_back("CLASSIFY");  // "classify" is only here to provide an example of "C"+[tab] completing to "CL" and displaying matches.
  autoScroll_ = true;
  scrollToBottom_ = true;
  addLog("Welcome to Dear ImGui!");
}

// Destructor
Console::~Console() {
  clearLog();
  for (int i = 0; i < history_.Size; i++)
  free(history_[i]);
}

// Prepare the console for drawing to the screen
void 
Console::create(const char* title, bool* p_open) {
  ImGui::SetNextWindowSize(ImVec2(520,600), ImGuiCond_FirstUseEver);
  if (!ImGui::Begin(title, p_open)) {
    ImGui::End();
    return;
  }

  // As a specific feature guaranteed by the library, after calling Begin() 
  // the last Item represent the title bar. So e.g. IsItemHovered() will return true when hovering the title bar.
  // Here we create a context menu only available from the title bar.
  if (ImGui::BeginPopupContextItem()) {
    if (ImGui::MenuItem("Close Console"))
      *p_open = false;
    ImGui::EndPopup();
  }

  ImGui::TextWrapped("This example implements a console with basic coloring, completion and history. A more elaborate implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
  ImGui::TextWrapped("Enter 'HELP' for help, press TAB to use text completion.");

  // TODO: display items starting from the bottom
  if (ImGui::SmallButton("Add Dummy Text"))  { 
    addLog("%d some text", items_.Size); 
    addLog("some more text"); 
    addLog("display very important message here!"); 
  } 
  ImGui::SameLine();
  if (ImGui::SmallButton("Add Dummy Error")) { 
    addLog("[error] something went wrong"); } 
  ImGui::SameLine();
  if (ImGui::SmallButton("Clear")) { 
    clearLog(); 
  } 
  ImGui::SameLine();
  bool copy_to_clipboard = ImGui::SmallButton("Copy"); 
  ImGui::SameLine();
  if (ImGui::SmallButton("Scroll to bottom")) scrollToBottom_ = true;
  //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

  ImGui::Separator();

  // Options menu
  if (ImGui::BeginPopup("Options")) {
    if (ImGui::Checkbox("Auto-scroll", &autoScroll_))
      if (autoScroll_)
        scrollToBottom_ = true;
    ImGui::EndPopup();
  }

  // Options, filter_
  if (ImGui::Button("Options"))
    ImGui::OpenPopup("Options");
  ImGui::SameLine();
  filter_.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
  ImGui::Separator();

  const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
  ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
  if (ImGui::BeginPopupContextWindow()) {
    if (ImGui::Selectable("Clear")) clearLog();
    ImGui::EndPopup();
  }

  // Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
  // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
  // You can seek and display only the lines that are visible using the ImGuiListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
  // To use the clipper we could replace the 'for (int i = 0; i < items_.Size; i++)' loop with:
  //     ImGuiListClipper clipper(items_.Size);
  //     while (clipper.Step())
  //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
  // However, note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
  // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
  // and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
  // If your items are of variable size you may want to implement code similar to what ImGuiListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
  if (copy_to_clipboard)
    ImGui::LogToClipboard();
  for (int i = 0; i < items_.Size; i++) {
    const char* item = items_[i];
    if (!filter_.PassFilter(item))
      continue;

    // Normally you would store more information in your item (e.g. make items_[] an array of structure, store color/type etc.)
    bool pop_color = false;
    if (strstr(item, "[error]"))            { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true; }
    else if (strncmp(item, "# ", 2) == 0)   { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f)); pop_color = true; }
    ImGui::TextUnformatted(item);
    if (pop_color)
      ImGui::PopStyleColor();
  }
  if (copy_to_clipboard)
    ImGui::LogFinish();
  if (scrollToBottom_)
    ImGui::SetScrollHereY(1.0f);
  scrollToBottom_ = false;
  ImGui::PopStyleVar();
  ImGui::EndChild();
  ImGui::Separator();

  // Command-line
  bool reclaim_focus = false;
  if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this)) {
    char* s = InputBuf;
    Strtrim(s);
    if (s[0])
      execCommand(s);
    strcpy(s, "");
    reclaim_focus = true;
  }

  // Auto-focus on window apparition
  ImGui::SetItemDefaultFocus();
  if (reclaim_focus)
    ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

  ImGui::End();
}


// Remove all items from the console
void 
Console::clearLog() {
  for (int i = 0; i < items_.Size; i++)
    free(items_[i]);
  items_.clear();
  scrollToBottom_ = true;
}

// Add something to the console
void 
Console::addLog(const char* fmt, ...) {
  // FIXME-OPT
  char buf[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
  buf[IM_ARRAYSIZE(buf)-1] = 0;
  va_end(args);
  items_.push_back(Strdup(buf));
  if (autoScroll_)
    scrollToBottom_ = true;
}

// Execute the entered command
void 
Console::execCommand(const char* command_line) {
  addLog("# %s\n", command_line);

  // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
  historyPos_ = -1;
  for (int i = history_.Size-1; i >= 0; i--)
    if (Stricmp(history_[i], command_line) == 0)
    {
      free(history_[i]);
      history_.erase(history_.begin() + i);
      break;
    }
  history_.push_back(Strdup(command_line));

  // Process command
  if (Stricmp(command_line, "CLEAR") == 0) {
    clearLog();
  }
  else if (Stricmp(command_line, "HELP") == 0) {
    addLog("commands_:");
    for (int i = 0; i < commands_.Size; i++)
      addLog("- %s", commands_[i]);
  }
  else if (Stricmp(command_line, "HISTORY") == 0) {
    int first = history_.Size - 10;
    for (int i = first > 0 ? first : 0; i < history_.Size; i++)
      addLog("%3d: %s\n", i, history_[i]);
  }
  else {
    addLog("Unknown command: '%s'\n", command_line);
  }

  // On commad input, we scroll to bottom even if autoScroll_==false
  scrollToBottom_ = true;
}

// Calculates possible matches
int 
Console::textEditCallback(ImGuiInputTextCallbackData* data) {
  //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
  switch (data->EventFlag) {
    case ImGuiInputTextFlags_CallbackCompletion: {
     // Example of TEXT COMPLETION

     // Locate beginning of current word
     const char* word_end = data->Buf + data->CursorPos;
     const char* word_start = word_end;
     while (word_start > data->Buf) {
       const char c = word_start[-1];
       if (c == ' ' || c == '\t' || c == ',' || c == ';')
         break;
       word_start--;
     }

     // Build a list of candidates
     ImVector<const char*> candidates;
     for (int i = 0; i < commands_.Size; i++)
       if (Strnicmp(commands_[i], word_start, (int)(word_end-word_start)) == 0)
         candidates.push_back(commands_[i]);

     if (candidates.Size == 0) {
       // No match
       addLog("No match for \"%.*s\"!\n", (int)(word_end-word_start), word_start);
     }
     else if (candidates.Size == 1) {
       // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
       data->DeleteChars((int)(word_start-data->Buf), (int)(word_end-word_start));
       data->InsertChars(data->CursorPos, candidates[0]);
       data->InsertChars(data->CursorPos, " ");
     }
     else {
       // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
       int match_len = (int)(word_end - word_start);
       for (;;) {
         int c = 0;
         bool all_candidates_matches = true;
         for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
           if (i == 0)
             c = toupper(candidates[i][match_len]);
           else if (c == 0 || c != toupper(candidates[i][match_len]))
             all_candidates_matches = false;
         if (!all_candidates_matches)
           break;
         match_len++;
       }

       if (match_len > 0) {
         data->DeleteChars((int)(word_start - data->Buf), (int)(word_end-word_start));
         data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
       }

       // List matches
       addLog("Possible matches:\n");
       for (int i = 0; i < candidates.Size; i++)
         addLog("- %s\n", candidates[i]);
     }

     break;
     }
    case ImGuiInputTextFlags_CallbackHistory: {
      // Example of HISTORY
      const int prev_history_pos = historyPos_;
      if (data->EventKey == ImGuiKey_UpArrow) {
        if (historyPos_ == -1)
          historyPos_ = history_.Size - 1;
        else if (historyPos_ > 0)
          historyPos_--;
      }
      else if (data->EventKey == ImGuiKey_DownArrow) {
        if (historyPos_ != -1)
          if (++historyPos_ >= history_.Size)
            historyPos_ = -1;
      }

      // A better implementation would preserve the data on the current input line along with cursor position.
      if (prev_history_pos != historyPos_) {
        const char* history_str = (historyPos_ >= 0) ? history_[historyPos_] : "";
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, history_str);
      }
    }
  }
  return 0;
}
