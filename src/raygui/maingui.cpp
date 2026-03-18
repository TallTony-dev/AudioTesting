#include "maingui.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib/raylib/include/raylib.h"
#include "../plugins/include/sequence.hpp"
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "../playback.hpp"
#include "../plugins/include/helpers.hpp"
bool isFirst = true;
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



void UpdateAndDrawTopBars() {
    if (!isClickUsed) {

    }
    DrawRectangle(0,0,(float)GetScreenWidth(), FILEBARHEIGHT, LIGHTGRAY);
    DrawRectangle(0,FILEBARHEIGHT,(float)GetScreenWidth(), TOOLBARHEIGHT, BROWN);

    //play button
    if (GuiButton({0,FILEBARHEIGHT,40,TOOLBARHEIGHT}, "play")) {
        playing = !playing;
    }
    //song load button


    
    //draw tool
    if (GuiButton({40,FILEBARHEIGHT,40,TOOLBARHEIGHT}, "draw")) {
        interactionState = INTERACT_ADDNOTE;
    }
    //default tool
    if (GuiButton({80,FILEBARHEIGHT,40,TOOLBARHEIGHT}, "normal")) {
        interactionState = INTERACT_DEFAULT;
    }
    //snap to note
    GuiCheckBox({120,FILEBARHEIGHT,40,TOOLBARHEIGHT}, "snap to beats", &snapToBeats);

    //save song button
    if (GuiButton({80,0,40,FILEBARHEIGHT}, "save song")) {
        for (Sequence *seq : sequenceBarseqs) {
            seq->SaveLoadedSong();
        }
    }
}

void UpdateAndDrawSideBar() {
    //have volume and mixing stuff here
    DrawRectangle(0,TOOLBARHEIGHT + FILEBARHEIGHT,SIDEBARTOTALWIDTH,GetScreenHeight() - TOOLBARHEIGHT - FILEBARHEIGHT, DARKBROWN);
    
    for (Sequence *seq : sequenceBarseqs) {
        
        
    }

    //plugin load button
    

}

#define TIMEBARRECT (Rectangle{SIDEBARTOTALWIDTH,(float)(FILEBARHEIGHT + TOOLBARHEIGHT), (float)GetScreenWidth() - SIDEBARTOTALWIDTH, (float)TIMEBARHEIGHT})
void DrawTimeBar() {
    //get significant digit
    float deltaTime = timeRightSide - timeLeftSide;
    int power = std::round(std::log10(deltaTime) - 0.8);
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
            DrawText(FloatToStr(time, std::clamp(-power, 0, 10)).c_str(), x + TIMETICKWIDTH + 1, y + TIMETICKHEIGHT - 10 , 10, BLACK);
        }
    }
    //draw current time line
    if (currentTime > timeLeftSide && currentTime < timeRightSide) {
        float timePos = ConvertTimeToXPos(currentTime) - TIMELINEWIDTH;
        DrawRectangle(timePos, y, TIMELINEWIDTH, GetScreenHeight() - y, BLACK);
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

int interactionState = INTERACT_DEFAULT;
bool snapToBeats = false;

void UpdateSequenceBars() {
    float deltaY = 0;
    if (!IsKeyDown(KEY_LEFT_SHIFT)) {
        //scroll here
        deltaY = GetMouseWheelMoveV().y * SCROLLSENSITIVITY;
    }
    Vector2 mousePos = GetMousePosition();
    for (Sequence* seq : sequenceBarseqs) {
        seq->seqYPos += deltaY;
        float seqYPos = seq->seqYPos;
        float currentSeqHeight = seq->seqHeight;
        if (seqYPos + currentSeqHeight > TOPBARTOTALHEIGHT && seqYPos < GetScreenHeight()) {
            Rectangle bottomRect = {SIDEBARTOTALWIDTH, seqYPos + currentSeqHeight, (float)GetScreenWidth() - SIDEBARTOTALWIDTH,SEQUENCEBARBORDERWIDTH};
            if (Intersects(bottomRect,mousePos)) {
                SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    seq->wasSeqResizeSelected = true;
                    isClickUsed = true;
                }
            }
            
            if (seq->wasSeqResizeSelected) {
                seq->seqHeight += GetMouseDelta().y;
                if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    seq->wasSeqResizeSelected = false;
                }
            }
            if (seq->seqHeight < MINSEQUENCEBARHEIGHT) {
                seq->seqHeight = MINSEQUENCEBARHEIGHT;
            }

            if (seq->highlightedSamp != nullptr) {
                delete seq->highlightedSamp->sample;
                delete seq->highlightedSamp;
                seq->highlightedSamp = nullptr;
            }
            if (Intersects({SIDEBARTOTALWIDTH, seqYPos, (float)GetScreenWidth() - SIDEBARTOTALWIDTH, seq->seqHeight}, mousePos)) {
                //manage click on actual sequence bar with draw tool here
                if (interactionState == INTERACT_ADDNOTE) {
                    seq->highlightedSamp = new SequenceSample();
                    seq->highlightedSamp->sample = new Sample();
                    seq->highlightedSamp->sample->length = 1;

                    if (!snapToBeats) {
                        seq->highlightedSamp->startTime = ConvertXPosToTime(mousePos.x);
                    }
                    else {
                        float tim = ConvertXPosToTime(mousePos.x);
                        std::tuple<Measure, float> *m = seq->GetMeasureAtTime(tim);
                        if (m != nullptr) {
                            Measure measure = std::get<0>(*m);
                            float mesStartTime = std::get<1>(*m);

                            float timepersubsec = measure.length / measure.denominator;
                            int beat = 0;
                            for (int i = 0; i < measure.denominator; i++) { 
                                if (tim > mesStartTime + timepersubsec * i && tim < mesStartTime + timepersubsec * (i + 1)) {
                                    beat = i;
                                    break;
                                }
                            }

                            seq->highlightedSamp->startTime = beat * timepersubsec + mesStartTime;
                            seq->highlightedSamp->sample->length = timepersubsec;
                        }
                    }
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        if (seq->highlightedSamp != nullptr) {
                            std::unordered_map<std::string, SampleProperty> props {{"vol", {1,0,1}}, {"freq", {50,0,500}}, {"len", {1,0,10}}};
                            seq->selectedSamp = seq->AddSamples(props, seq->highlightedSamp->startTime);
                        }
                    }

                }
                //manage selecting different sequence samps here
                else if (interactionState == INTERACT_DEFAULT) {
                    for (DrawnSample samp : seq->lastDrawnSamples) {
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && Intersects(samp.rect, mousePos)) {
                            seq->selectedSamp = samp.samp;
                            isClickUsed = true;
                        }
                    }
                }
            }
        }
    }
}

void DrawSequenceBars() {
    float cumHeight = 0;

    for (Sequence *seq : sequenceBarseqs) {
        seq->lastDrawnSamples.clear();
        std::vector<SequenceSample*> samps = seq->GetAllSamples();
        std::vector<std::tuple<Measure, float>> tups = seq->GetMeasures();
        float screenHeight = GetScreenHeight();
        float screenWidth = GetScreenWidth() - SIDEBARTOTALWIDTH;
        float height = seq->seqHeight;
        float yTop = seq->seqYPos;

        seq->seqYPos = cumHeight + TOPBARTOTALHEIGHT;
        cumHeight += (height + SEQUENCEBARBORDERWIDTH);
        float x = SIDEBARTOTALWIDTH;
        
        DrawRectangle(x, yTop, screenWidth, height, LIGHTGRAY);
        DrawRectangle(x, yTop + height, GetScreenWidth() - SIDEBARTOTALWIDTH, SEQUENCEBARBORDERWIDTH, BLACK);
        DrawText(seq->name.c_str(), x, yTop, 20, BLACK);
        //draw measures
        for (std::tuple<Measure, float> tup : tups) {
            Measure measure = std::get<0>(tup);
            float mesStartTime = std::get<1>(tup);
            float mesx = ConvertTimeToXPos(mesStartTime);
            if (ConvertTimeToXPos(mesStartTime + measure.length) >= x && mesx <= screenWidth) {
                DrawRectangle(mesx, yTop, MEASUREBARWIDTH, height, GREEN);
                float xpersubsec = (ConvertTimeToXPos(measure.length + mesStartTime) - ConvertTimeToXPos(mesStartTime)) / measure.denominator;
                for (int i = 1; i < measure.denominator; i++) {
                    DrawRectangle(mesx + i * xpersubsec, yTop, BEATBARWIDTH, height, DARKGRAY);
                }

            }
        }
        //draw samples
        std::vector allSamps = seq->GetAllSamples();
        if (seq->highlightedSamp != nullptr) {
            allSamps.push_back(seq->highlightedSamp);
        }

        for (auto i = allSamps.begin(); i != allSamps.end(); i++) {
            SequenceSample *samp = *i;
            float sampx = ConvertTimeToXPos(samp->startTime);
            float sampy = yTop;
            float sampWidth = ConvertTimeToXPos(samp->startTime + samp->sample->length) - sampx;
            float sampHeight = height;
            float freq = samp->sample->freq;
            if (freq > 0) {
                sampHeight = 3;
                sampy = yTop + height - sampHeight - std::clamp((freq - 100) / 5, 0.0f, height);
            }
            else {
                //TODO: handle non freq based drawing here
            }
            

            if (sampx + sampWidth >= x && sampx <= screenWidth + x) {
                DrawRectangle(sampx, sampy, sampWidth, sampHeight, DARKGREEN);
                seq->lastDrawnSamples.push_back({samp, {sampx, sampy, sampWidth, sampHeight}});
            }
        }
    }
    DrawTimeBar();
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

bool CreateWindowBoxAround(Rectangle rect, std::string name) {
    const float borderWidth = 1;
    Rectangle guiBox = {.width = rect.width + borderWidth * 2, .height = rect.height + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, 
    .x = rect.x - borderWidth, .y = rect.y - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + borderWidth};
    return GuiWindowBox(guiBox, name.c_str());
}

void ManageZooming() {
    float deltaTime = timeRightSide - timeLeftSide;
    Vector2 scroll = GetMouseWheelMoveV();
    float biggerScroll = GetMouseWheelMove();
    if (scroll.x != 0 && scroll.x == biggerScroll) {
        timeLeftSide -= deltaTime * scroll.x * TIMESCROLLSENSITIVITY;
        timeRightSide -= deltaTime * scroll.x * TIMESCROLLSENSITIVITY;
    }
    if (scroll.y != 0 && scroll.y == biggerScroll && IsKeyDown(KEY_LEFT_SHIFT)) {
        float newLeft = timeLeftSide + deltaTime * scroll.y * TIMESCALESENSITIVITY;
        float newRight = timeRightSide - deltaTime * scroll.y * TIMESCALESENSITIVITY;
        //don't zoom tooooo far
        if (newRight - newLeft > 0.001f && newRight - newLeft < 80) {
            timeLeftSide = newLeft;
            timeRightSide = newRight;
        }

    }
}

void UpdateGui(PluginLoader &plugins) {
    if (IsWindowResized() || isFirst) {
        Vector2 window = {(float)GetScreenWidth(),(float)GetScreenHeight()};
        // playButtonRect = {window.x * 0.5f - PLAYBUTTONWIDTH, TOPBARTOTALHEIGHT,PLAYBUTTONWIDTH, PLAYBUTTONHEIGHT};
    }
    isClickUsed = false;
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    UpdateWindowSelection(plugins);
    DrawSequenceBars();
    UpdateSequenceBars();
    UpdateTimeBar();
    UpdateAndDrawTopBars();
    UpdateAndDrawSideBar();
    ManageZooming();
    isFirst = false;
}