// Console.h
// Class to manage IMGUI console for debugging

#ifndef CONSOLE_H
#define CONSOLE_H

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

// Demonstrate creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, here we are using a more C++ like approach of declaring a class to hold the data and the functions.
class Console {

  public:

    // Portable helpers
    static int Stricmp(const char* str1, const char* str2) { 
      int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; 
    }
    static int Strnicmp(const char* str1, const char* str2, int n) { 
      int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; 
    }
    static char* Strdup(const char *str) { 
      size_t len = strlen(str) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)str, len); 
    }
    static void  Strtrim(char* str) { 
      char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; 
    }

    // In C++11 you are better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data) {
      Console* console = (Console*)data->UserData;
      return console->textEditCallback(data);
    }

    // Constructor
    Console();

    // Destructor
    ~Console();
    
    // Prepare the console for drawing to the screen
    void create(const char* title, bool* p_open);

  private:

    char InputBuf[256];
    ImVector<char*> items_;
    ImVector<const char*> commands_;
    ImVector<char*> history_;
    int historyPos_;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter filter_;
    bool autoScroll_;
    bool scrollToBottom_;

    // Remove all items from the console
    void clearLog();

    // Add something to the console
    void addLog(const char* fmt, ...) IM_FMTARGS(2);

    // Execute the entered command
    void execCommand(const char* command_line);
    
    // Calculates possible matches
    int textEditCallback(ImGuiInputTextCallbackData* data);
};

#endif
