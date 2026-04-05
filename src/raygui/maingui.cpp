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

double timeLeftSide = 0;
double timeRightSide = 5;
float ConvertTimeToXPos(double time) {
    double deltaTime = timeRightSide - timeLeftSide;
    float width = GetScreenWidth() - SIDEBARTOTALWIDTH;
    float scale = width / deltaTime;
    return scale * (time - timeLeftSide) + SIDEBARTOTALWIDTH;
}
float ConvertXPosToTime(float xPos) {
    double deltaTime = timeRightSide - timeLeftSide;
    xPos -= SIDEBARTOTALWIDTH;
    float width = GetScreenWidth() - SIDEBARTOTALWIDTH;
    float scale = width / deltaTime;
    return (xPos / scale) + timeLeftSide;
}
float ConvertDeltaXPosToTime(float xPos) {
    double deltaTime = timeRightSide - timeLeftSide;
    float width = GetScreenWidth();
    float scale = width / deltaTime;
    return (xPos / scale) + timeLeftSide;
}

//returns the length of a beat in the nearest measure to a time
float GetBeatLengthAtTime(double time, Sequence *seq) {
    std::tuple<Measure, double> *m = seq->GetMeasureAtTime(time);
    if (m != nullptr) {
        Measure measure = std::get<0>(*m);
        double mesStartTime = std::get<1>(*m);

        return measure.length / measure.denominator;
    }
    return 1;
}
//returns the starttime of the nearest beat to the given time and seq
float SnapToBeat(double time, Sequence *seq) {
    std::tuple<Measure, double> *m = seq->GetMeasureAtTime(time);
    if (m != nullptr) {
        Measure measure = std::get<0>(*m);
        double mesStartTime = std::get<1>(*m);

        double timepersubsec = measure.length / measure.denominator;
        int beat = 0;
        for (int i = 0; i < measure.denominator; i++) { 
            if (time > mesStartTime + timepersubsec * i && time < mesStartTime + timepersubsec * (i + 1)) {
                beat = i;
                break;
            }
        }

        return beat * timepersubsec + mesStartTime;
    }
    return time;
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
    if (GuiButton({120,FILEBARHEIGHT,40,TOOLBARHEIGHT}, "delete")) {
        interactionState = INTERACT_DELETENOTE;
    }
    //snap to note
    GuiCheckBox({160,FILEBARHEIGHT,40,TOOLBARHEIGHT}, "snap to beats", &snapToBeats);

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
    float cumHeight = TOPBARTOTALHEIGHT;
    for (Sequence *seq : sequenceBarseqs) {
        GuiSliderBar({0, cumHeight, 20, 10}, "", "volume", &seq->seqVolume, 0, 1);

        cumHeight += seq->seqHeight;
    }

    //plugin add to song button
    

}

#define TIMEBARRECT (Rectangle{SIDEBARTOTALWIDTH,(float)(FILEBARHEIGHT + TOOLBARHEIGHT), (float)GetScreenWidth() - SIDEBARTOTALWIDTH, (float)TIMEBARHEIGHT})
void DrawTimeBar() {
    //get significant digit
    double deltaTime = timeRightSide - timeLeftSide;
    int power = std::round(std::log10(deltaTime) - 0.8);
    float sig = std::pow(10, power);
    double firstTime;
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
    for (double time = firstTime; time < firstTime + sig * 30; time += sig) {
        if (time > timeLeftSide && time < timeRightSide) {
            float x = ConvertTimeToXPos(time);
            DrawRectangle(x, y, TIMETICKWIDTH, TIMETICKHEIGHT, PURPLE);
            DrawText(FloatToStr(time, std::clamp(-power, 0, 10)).c_str(), x + TIMETICKWIDTH + 1, y + TIMETICKHEIGHT - 10 , 10, BLACK);
        }
    }
    //draw current time line
    if (currentTime > timeLeftSide && currentTime < timeRightSide) {
        double timePos = ConvertTimeToXPos(currentTime) - TIMELINEWIDTH;
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
        seq->highlightedSamp = nullptr;
        seq->seqYPos += deltaY;
        float seqYPos = seq->seqYPos;
        float currentSeqHeight = seq->seqHeight;
        if (seqYPos + currentSeqHeight > TOPBARTOTALHEIGHT && seqYPos < GetScreenHeight()) {
            Rectangle bottomRect = {SIDEBARTOTALWIDTH, seqYPos + currentSeqHeight, (float)GetScreenWidth() - SIDEBARTOTALWIDTH,SEQUENCEBARBORDERWIDTH};
            if (Intersects(bottomRect,mousePos)) {
                SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isClickUsed) {
                    seq->wasSeqResizeSelected = true;
                    isClickUsed = true;
                }
            }
            
            if (seq->wasSeqResizeSelected) {
                if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    seq->wasSeqResizeSelected = false;
                    break;
                }
                seq->seqHeight += GetMouseDelta().y;
            }

            if (seq->wasSelectedSampMoveSelected) {
                if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    seq->wasSelectedSampMoveSelected = false;
                    break;
                }
                if (!snapToBeats) {
                    for (SequenceSample *samp : seq->selectedSamps) {
                        samp->startTime += ConvertDeltaXPosToTime(GetMouseDelta().x);
                    }
                }
                else {
                    for (SequenceSample *samp : seq->selectedSamps) {
                        samp->startTime += SnapToBeat(ConvertXPosToTime(mousePos.x), seq) - SnapToBeat(ConvertXPosToTime(mousePos.x - GetMouseDelta().x), seq);
                    }
                }
            }

            if (seq->seqHeight < MINSEQUENCEBARHEIGHT) {
                seq->seqHeight = MINSEQUENCEBARHEIGHT;
            }

            if (seq->ghostSamp != nullptr) {
                delete seq->ghostSamp->sample;
                delete seq->ghostSamp;
                seq->ghostSamp = nullptr;
            }
            if (Intersects({SIDEBARTOTALWIDTH, seqYPos, (float)GetScreenWidth() - SIDEBARTOTALWIDTH, seq->seqHeight}, mousePos)) {
                //manage click on actual sequence bar with draw tool here
                if (interactionState == INTERACT_ADDNOTE) {
                    seq->ghostSamp = new SequenceSample();
                    seq->ghostSamp->sample = new Sample();
                    seq->ghostSamp->sample->length = 1;

                    if (!snapToBeats) {
                        seq->ghostSamp->startTime = ConvertXPosToTime(mousePos.x);
                    }
                    else {
                        float tim = ConvertXPosToTime(mousePos.x);
                        seq->ghostSamp->startTime = SnapToBeat(tim, seq);
                        seq->ghostSamp->sample->length = GetBeatLengthAtTime(tim, seq);
                    }
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isClickUsed) {
                        if (seq->ghostSamp != nullptr) {
                            seq->selectedSamps.clear();
                            std::unordered_map<std::string, SampleProperty> props {{"vol", {1,0,1}}, {"freq", {50,0,500}}, {"len", {1,0,10}}};
                            seq->selectedSamps.push_back(seq->AddSamples(props, seq->ghostSamp->startTime));
                        }
                    }

                }
                //manage selecting different sequence samps here
                else if (interactionState == INTERACT_DEFAULT) {
                    bool wasOneSelected = false;
                    for (DrawnSample samp : seq->lastDrawnSamples) {
                        if (Intersects(samp.rect, mousePos)) {
                            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isClickUsed) {
                                //if not in the selectedsamps
                                if (std::find(seq->selectedSamps.begin(), seq->selectedSamps.end(), samp.samp) == seq->selectedSamps.end()) {
                                    seq->selectedSamps.push_back(samp.samp);
                                    wasOneSelected = true;
                                }
                                else {
                                    seq->wasSelectedSampMoveSelected = true;
                                    wasOneSelected = true;
                                }
                                isClickUsed = true;
                            }
                            else {
                                seq->highlightedSamp = samp.samp;
                            }
                        }
                    }
                    if (!wasOneSelected && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isClickUsed) {
                        seq->selectedSamps.clear();
                        isClickUsed = true;
                    }
                    DrawText(std::to_string(seq->selectedSamps.size()).c_str(), 700,300, 50, BLACK);
                }
                else if (interactionState == INTERACT_DELETENOTE) {
                    for (DrawnSample samp : seq->lastDrawnSamples) {
                        if (Intersects(samp.rect, mousePos)) {
                            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isClickUsed) {
                                isClickUsed = true;
                                seq->RemoveSample(samp.samp);
                                break;
                            }
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
        std::vector<std::tuple<Measure, double>> tups = seq->GetMeasures();
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
        for (std::tuple<Measure, double> tup : tups) {
            Measure measure = std::get<0>(tup);
            double mesStartTime = std::get<1>(tup);
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

        if (seq->ghostSamp != nullptr) {
            //add to the list because it normally isnt there
            allSamps.push_back(seq->ghostSamp);
        }

        for (auto i = allSamps.begin(); i != allSamps.end(); i++) {
            SequenceSample *samp = *i;
            bool isSelectedSamp = seq->selectedSamps.end() != std::find(seq->selectedSamps.begin(), seq->selectedSamps.end(), samp);
            bool isHighlightedSamp = seq->highlightedSamp == samp;
            float sampx = ConvertTimeToXPos(samp->startTime);
            float sampy = yTop;
            float sampWidth = ConvertTimeToXPos(samp->startTime + samp->sample->length) - sampx;
            float sampHeight = height;
            Color col = DARKGREEN;
            float freq = samp->sample->freq;
            if (freq > 0) {
                sampHeight = 3;
                sampy = yTop + height - sampHeight - std::clamp((freq - 100) / 5, 0.0f, height);
            }
            else {
                //TODO: handle non freq based drawing here


            }
            
            if (isSelectedSamp) {
                col = DARKPURPLE;
            }
            if (isHighlightedSamp) {
                col = {0, 100, 230, 255};
                if (isSelectedSamp) {
                    col = GREEN;
                }
                sampy -= 2;
                sampHeight += 4;
            }

            if (sampx + sampWidth >= x && sampx <= screenWidth + x) {
                DrawRectangle(sampx, sampy, sampWidth, sampHeight, col);
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
            if (seq->isWindowShown && seq->hasWindow) {
                if (x == plugins.plugins.end() - 1) {
                    SetWindowSelected(seq); //just in case it isnt already, very cheap op
                    if (Intersects(seq->GetCurrentWindowPaddedPos(), mousePos)) {
                        isClickUsed = true;
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
}

bool CreateWindowBoxAround(Rectangle rect, std::string name) {
    const float borderWidth = 1;
    Rectangle guiBox = {.width = rect.width + borderWidth * 2, .height = rect.height + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, 
    .x = rect.x - borderWidth, .y = rect.y - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + borderWidth};
    return GuiWindowBox(guiBox, name.c_str());
}

void ManageZooming() {
    double deltaTime = timeRightSide - timeLeftSide;
    Vector2 scroll = GetMouseWheelMoveV();
    float biggerScroll = GetMouseWheelMove();
    if (scroll.x != 0 && scroll.x == biggerScroll) {
        timeLeftSide -= deltaTime * scroll.x * TIMESCROLLSENSITIVITY;
        timeRightSide -= deltaTime * scroll.x * TIMESCROLLSENSITIVITY;
    }
    if (scroll.y != 0 && scroll.y == biggerScroll && IsKeyDown(KEY_LEFT_SHIFT)) {
        double newLeft = timeLeftSide + deltaTime * scroll.y * TIMESCALESENSITIVITY;
        double newRight = timeRightSide - deltaTime * scroll.y * TIMESCALESENSITIVITY;
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
    UpdateWindowSelection(plugins);
    DrawSequenceBars();
    UpdateSequenceBars();
    UpdateTimeBar();
    UpdateAndDrawTopBars();
    UpdateAndDrawSideBar();
    ManageZooming();
    isFirst = false;
}