// Console.cpp
// Class to manage IMGUI console for debugging

#include "Console.h"

// Avoid cyclic dependencies
#include "Game.h"

// Initialise static variables
char Console::InputBuf[256];
ImVector<char*> Console::items_;
ImVector<const char*> Console::commands_;
ImVector<char*> Console::history_;
int Console::historyPos_;    // -1: new line, 0..History.Size-1 browsing history.
ImGuiTextFilter Console::filter_;
bool Console::autoScroll_;
bool Console::scrollToBottom_;
bool Console::outputToTerminal_ = false;

// Start the console
void
Console::initialise(bool outputToTerminal) {
  clear();
  memset(InputBuf, 0, sizeof(InputBuf));
  historyPos_ = -1;
  addCommand("help");
  addCommand("clear");
  addCommand("history");
  addCommand("print");
  autoScroll_ = true;
  scrollToBottom_ = true;

  // Set whether we should output to terminal
  setOutputToTerminal(outputToTerminal);
}

// Shut the console down
void
Console::shutdown() {
  clear();
  for (int i = 0; i < history_.Size; ++i)
  free(history_[i]);
}

// Add commands to the auto completion
void
Console::addCommand(const char* command) {
  commands_.push_back(command);
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
    if (ImGui::MenuItem("Close Console")) { *p_open = false; }
    ImGui::EndPopup();
  }

  // Text at the top
  ImGui::TextWrapped("This console allows you to interact with the application's Lua environment.");
  ImGui::TextWrapped("For a list of basic commands, type 'HELP'.");

  // Clear the log on click
  if (ImGui::SmallButton("Clear")) { clear(); } 
  ImGui::SameLine();

  // Copy to clipboard button
  bool copy_to_clipboard = ImGui::SmallButton("Copy"); 
  ImGui::SameLine();

  // Scroll to bottom button
  if (ImGui::SmallButton("Scroll to bottom")) { scrollToBottom_ = true; }
  ImGui::Separator();

  // Options menu
  if (ImGui::BeginPopup("Options")) {
    if (ImGui::Checkbox("Auto-scroll", &autoScroll_)) {
      if (autoScroll_) scrollToBottom_ = true;
    }
    ImGui::EndPopup();
  }

  // Options, filter_
  if (ImGui::Button("Options")) { ImGui::OpenPopup("Options"); }
  ImGui::SameLine();

  // Allow filtering of text
  filter_.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
  ImGui::Separator();

  // Reserve space for scrolling area
  // 1 separator, 1 input text
  const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); 
  ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

  // Allow the user to clear the log by right clicking the scrollable area
  if (ImGui::BeginPopupContextWindow()) {
    if (ImGui::Selectable("Clear")) { clear(); }
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
  for (int i = 0; i < items_.Size; ++i) {
    const char* item = items_[i];
    if (!filter_.PassFilter(item))
      continue;

    // Normally you would store more information in your item (e.g. make items_[] an array of structure, store color/type etc.)
    bool pop_color = false;
    if (strstr(item, "[Error]"))            { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true; }
    else if (strstr(item, "[Warning]"))            { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); pop_color = true; }
    else if (strstr(item, "[Class]"))            { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f)); pop_color = true; }
    else if (strstr(item, "[Note]"))            { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f)); pop_color = true; }
    else if (strncmp(item, "$ ", 2) == 0)   { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f)); pop_color = true; }
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

  // Text input area
  bool reclaim_focus = false;
  if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub)) {
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

  // End window
  ImGui::End();
}


// Remove all items from the console
void 
Console::clear() {
  for (int i = 0; i < items_.Size; ++i)
    free(items_[i]);
  items_.clear();
  scrollToBottom_ = true;
}

// Add something to the console
void 
Console::log(const char* fmt, ...) {

  // Add log entry
  char buf[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
  buf[IM_ARRAYSIZE(buf)-1] = 0;
  va_end(args);

  // Duplicate the string into a message
  auto msg = Strdup(buf);
  items_.push_back(msg);

  // Print to terminal
  if (outputToTerminal_) {
    printf("%s\n", buf);
  }

  // Scroll to the bottom if desired
  if (autoScroll_) {
    scrollToBottom_ = true;
  }
}

// Execute the entered command
void 
Console::execCommand(const char* command_line) {

  // Get a string from the command
  const std::string command = command_line;

  // Log the entered command
  log("$ %s\n", command.c_str());

  // Insert into history
  // First find match and delete it so it can be pushed to the back
  historyPos_ = -1;
  for (int i = history_.Size-1; i >= 0; --i) {
    if (Stricmp(history_[i], command_line) == 0) {
      free(history_[i]);
      history_.erase(history_.begin() + i);
      break;
    }
  }
  history_.push_back(Strdup(command_line));

  // If the command was 'Help' print basic commands
  if (Stricmp(command_line, "HELP") == 0) {
    log("Commands:");
    for (int i = 0; i < commands_.Size; ++i) {
      log("- %s", commands_[i]);
    }
  }

  // If the command was 'History', show history
  else if (Stricmp(command_line, "HISTORY") == 0) {
    int first = history_.Size - 10;
    for (int i = first > 0 ? first : 0; i < history_.Size; ++i) {
      log("%3d: %s", i, history_[i]);
    }
  }

  // Clear the log with the CLEAR command
  if (Stricmp(command_line, "CLEAR") == 0) {
    clear();
  }

  // Otherwise, run the command
  else {
    sol::protected_function_result result = Game::lua.script(command, sol::script_pass_on_error);
    if (!result.valid()) {
      sol::error err = result;
      log("[Error] Unknown command: %s\n> %s", command.c_str(), err.what());
    }
  }

  // On commad input, we scroll to bottom even if autoScroll_==false
  scrollToBottom_ = true;
}

// Get whether we should output to terminal
bool
Console::getOutputToTerminal() {
  return outputToTerminal_;
}

// Set whether we should output to terminal
void
Console::setOutputToTerminal(bool enable) {
  Console::log("Output to terminal: %s", enable ? "enabled" : "disabled");
  outputToTerminal_ = enable;
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
     for (int i = 0; i < commands_.Size; ++i)
       if (Strnicmp(commands_[i], word_start, (int)(word_end-word_start)) == 0) {
         candidates.push_back(commands_[i]);
        }

     if (candidates.Size == 0) {
       // No match
       log("No match for \"%.*s\"!", (int)(word_end-word_start), word_start);
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
         for (int i = 0; i < candidates.Size && all_candidates_matches; ++i)
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
       log("Possible matches:");
       for (int i = 0; i < candidates.Size; ++i) {
         log("- %s", candidates[i]);
       }
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
