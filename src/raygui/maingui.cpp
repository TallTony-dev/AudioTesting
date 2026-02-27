#include "maingui.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"
#include "../plugins/include/sequence.hpp"
#include <vector>
#include "../plugins/include/helpers.hpp"

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
void DrawSequenceBars() {
    int currentInd = 0;
    for (Sequence *seq : sequenceBarseqs) {
        std::vector<SequenceSample*> samps = seq->GetAllSamples();
        std::vector<std::tuple<Measure, float>> tups = seq->GetMeasures();
        float screenHeight = GetScreenHeight();
        float screenWidth = GetScreenWidth();
        float height = SEQUENCEBARHEIGHT;
        GuiPanel(Rectangle{0, height * currentInd, screenWidth, height}, seq->name.c_str());
        for (std::tuple<Measure, float> tup : tups) {
            Measure measure = std::get<0>(tup);
            float mesStartTime = std::get<1>(tup);
            
        }
        currentInd++;
    }
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