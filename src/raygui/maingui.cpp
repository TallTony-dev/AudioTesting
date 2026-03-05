#include "maingui.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"
#include "../plugins/include/sequence.hpp"
#include <vector>
#include <sstream>
#include <iomanip>
#include "../playback.hpp"
#include "../plugins/include/helpers.hpp"

bool isClickUsed = false;

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

std::vector<Sequence *> sequenceBarseqs;
void AddToSequenceBar(Sequence *seq) {
    sequenceBarseqs.push_back(seq);
}
bool RemoveFromSequenceBar(Sequence *seq) {
    for (auto x = sequenceBarseqs.begin(); x != sequenceBarseqs.end(); x++) {
        if (*x == seq) {
            sequenceBarseqs.erase(x);
            return true;
        }
    }
    return false;
}

float timeLeftSide = 0;
float timeRightSide = 5;
float ConvertTimeToXPos(float time) {
    float deltaTime = timeRightSide - timeLeftSide;
    float width = GetScreenWidth() - SIDEBARTOTALWIDTH;
    float scale = width / deltaTime;
    return scale * (time - timeLeftSide) + SIDEBARTOTALWIDTH;
}
float ConvertXPosToTime(float xPos) {
    float deltaTime = timeRightSide - timeLeftSide;
    xPos -= SIDEBARTOTALWIDTH;
    float width = GetScreenWidth() - SIDEBARTOTALWIDTH;
    float scale = width / deltaTime;
    return (xPos / scale) + timeLeftSide;
}

std::string inline FloatToStr(float val, int precision) {
    std::stringstream str;
    str << std::fixed << std::setprecision(precision) << val;
    return str.str();
}

#define TIMEBARRECT (Rectangle{SIDEBARTOTALWIDTH,(float)(FILEBARHEIGHT + TOOLBARHEIGHT), (float)GetScreenWidth() - SIDEBARTOTALWIDTH, (float)TIMEBARHEIGHT})
void DrawTimeBar() {
    //get significant digit
    float deltaTime = timeRightSide - timeLeftSide;
    int power = std::round(std::log10(deltaTime) - 0.9);
    float sig = std::pow(10, power);
    float firstTime;
    if (sig != 0) {
        firstTime = std::round(timeLeftSide / sig) * sig; 
    }
    else {
        firstTime = std::round(timeLeftSide);
    }

    float y = (FILEBARHEIGHT + TOOLBARHEIGHT);

    //draw bg
    DrawRectangleRec(TIMEBARRECT, GRAY);
    //draw major ticks
    for (float time = firstTime; time < firstTime + sig * 30; time += sig) {
        if (time > timeLeftSide && time < timeRightSide) {
            float x = ConvertTimeToXPos(time);
            DrawRectangle(x, y, TIMETICKWIDTH, TIMETICKHEIGHT, PURPLE);
            DrawText(FloatToStr(time, 4).c_str(), x, TIMETICKHEIGHT, 10, BLACK);
        }
    }
}
void UpdateTimeBar() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isClickUsed) {
        if (Intersects(TIMEBARRECT, GetMousePosition())) {
            currentTime = ConvertXPosToTime(GetMouseX());
            isClickUsed = true;
        }
    }
}

void DrawSequenceBars() {
    int currentInd = 0;
    for (Sequence *seq : sequenceBarseqs) {
        std::vector<SequenceSample*> samps = seq->GetAllSamples();
        std::vector<std::tuple<Measure, float>> tups = seq->GetMeasures();
        float screenHeight = GetScreenHeight();
        float screenWidth = GetScreenWidth() - SIDEBARTOTALWIDTH;
        float height = SEQUENCEBARHEIGHT;
        float x = SIDEBARTOTALWIDTH;
        float yTop = height * currentInd + TOPBARTOTALHEIGHT;
        
        GuiPanel(Rectangle{x, yTop, screenWidth, height}, seq->name.c_str());
        //draw measures
        for (std::tuple<Measure, float> tup : tups) {
            Measure measure = std::get<0>(tup);
            float mesStartTime = std::get<1>(tup);
            float mesx = ConvertTimeToXPos(mesStartTime);
            if (mesx >= x && mesx <= screenWidth) {
                DrawRectangle(mesx, yTop, MEASUREBARWIDTH, height, GREEN);
            }
        }
        //draw current time line
        if (currentTime > timeLeftSide && currentTime < timeRightSide) {
            float timePos = ConvertTimeToXPos(currentTime);
            DrawRectangle(timePos, TOPBARTOTALHEIGHT, TIMELINEWIDTH, GetScreenHeight() - TOPBARTOTALHEIGHT, BLACK);
        }
        currentInd++;
    }
    DrawTimeBar();
}

Rectangle playButtonRect;
bool DrawPlayButton() {
    return GuiLabelButton(playButtonRect, "play");
}

Sequence *selectedSequence;
bool IsWindowSelected(Sequence *window) {
    return (window == selectedSequence);
}
void SetWindowSelected(Sequence *window) {
    selectedSequence = window;
}
//check for if mouse is clicked at a point which doesnt intersect selected window but does select another
//also make sure that the last window drawn is the one selected as it is the first shown
void UpdateWindowSelection(PluginLoader &plugins) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();

        for (auto x = plugins.plugins.end(); x != plugins.plugins.begin();) {
            x--;
            LoadedPlugin plugin = (*x);
            Sequence *seq = plugin.sequence;
            if (x == plugins.plugins.end() - 1) {
                SetWindowSelected(seq); //just in case it isnt already, very cheap op
                if (Intersects(seq->GetCurrentWindowPaddedPos(), mousePos)) {
                    break;
                }
            }
            else if (Intersects(seq->GetCurrentWindowPos(), mousePos)) {
                //Move that sequence to the back of the sequence list
                plugins.plugins.erase(x);
                plugins.plugins.push_back(plugin);
                SetWindowSelected(seq);
                isClickUsed = true;
            }
        }
    }
}

bool DrawWindowBoxAround(Rectangle rect, std::string name) {
    const float borderWidth = 1;
    Rectangle guiBox = {.width = rect.width + borderWidth * 2, .height = rect.height + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, 
    .x = rect.x - borderWidth, .y = rect.y - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + borderWidth};
    return GuiWindowBox(guiBox, name.c_str());
}

bool isFirst = true;
void UpdateGui(PluginLoader &plugins) {
    if (IsWindowResized() || isFirst) {
        Vector2 window = {(float)GetScreenWidth(),(float)GetScreenHeight()};
        playButtonRect = {window.x * 0.5f - PLAYBUTTONWIDTH, TOPBARTOTALHEIGHT,PLAYBUTTONWIDTH, PLAYBUTTONHEIGHT};
        isFirst = false;
    }
    isClickUsed = false;
    UpdateWindowSelection(plugins);
    UpdateTimeBar();

    float deltaTime = timeRightSide - timeLeftSide;
    Vector2 scroll = GetMouseWheelMoveV();
    float biggerScroll = GetMouseWheelMove();
    if (scroll.x != 0 && scroll.x == biggerScroll) {
        timeLeftSide -= deltaTime * scroll.x * TIMESCROLLSENSITIVITY;
        timeRightSide -= deltaTime * scroll.x * TIMESCROLLSENSITIVITY;
    }
    if (scroll.y != 0 && scroll.y == biggerScroll) {
        float newLeft = timeLeftSide + deltaTime * scroll.y * TIMESCALESENSITIVITY;
        float newRight = timeRightSide - deltaTime * scroll.y * TIMESCALESENSITIVITY;
        //don't zoom tooooo far
        if (newRight - newLeft > 0.001f) {
            timeLeftSide = newLeft;
            timeRightSide = newRight;
        }
    }
}