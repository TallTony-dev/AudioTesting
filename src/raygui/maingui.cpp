#include "maingui.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"
#include "../plugins/include/sequence.hpp"
#include <vector>

int bottomBarIndexCount;
bool BottomBarButton(std::string name, int barIndex) {
    return GuiButton({.width = BOTTOMBARWIDTH, .height = BOTTOMBARHEIGHT, .x = (float)BOTTOMBARWIDTH * barIndex, .y = (float)GetScreenHeight() - BOTTOMBARHEIGHT},
    name.c_str());
}

std::vector<Sequence *> bottomBarSequences;
void AddToBottomBar(Sequence *seq) {
    bottomBarSequences.push_back(seq);
}
void RemoveFromBottomBar(Sequence *seq) {
    for (auto x = bottomBarSequences.begin(); x != bottomBarSequences.end(); x++) {
        if (*x == seq) {
            bottomBarSequences.erase(x);
            break;
        }
    }
}
void DrawBottomBar() {
    int bottomBarCount = 0;
    for (Sequence *seq : bottomBarSequences) {
        if (BottomBarButton(seq->name, bottomBarCount)) {
            seq->isWindowShown = true;
            RemoveFromBottomBar(seq);
        }
        bottomBarCount++;
    }
}

bool DrawWindowBoxAround(Rectangle rect, std::string name) {
    const float borderWidth = 1;
    Rectangle guiBox = {.width = rect.width + borderWidth * 2, .height = rect.height + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, 
    .x = rect.x - borderWidth, .y = rect.y - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + borderWidth};
    return GuiWindowBox(guiBox, "pluginwindow");
}