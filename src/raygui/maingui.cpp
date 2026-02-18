#include "maingui.hpp"
#include "raygui.h"
#include "raylib.h"

bool BottomBarButton(std::string name, int barIndex) {
    return GuiButton({.width = BOTTOMBARWIDTH, .height = BOTTOMBARHEIGHT, .x = (float)BOTTOMBARWIDTH * barIndex, .y = (float)GetScreenHeight() - BOTTOMBARHEIGHT},
    name.c_str());
}