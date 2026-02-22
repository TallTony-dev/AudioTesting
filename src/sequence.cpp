#include "plugins/include/sequence.hpp"
#include "plugins/include/sample.hpp"
#include "raygui/maingui.hpp"
#include "raygui/raygui.h"
#include "plugins/include/helpers.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <ranges>
#include <string_view>
#include <string>
#include <algorithm>
#include <exception>


Sequence::~Sequence() {
    for (SequenceSample samp : activeSamples) {
        delete samp.sample;
    }
    for (SequenceSample samp : samplesToAdd) {
        delete samp.sample;
    }
    for (SequenceSample samp : samplesAdded) {
        delete samp.sample;
    }
    if (windowTex.id != 0)
        UnloadRenderTexture(windowTex);
}   

void Sequence::UpdateCurrentPos(Rectangle rect) {
    if (rect.width < SEQWINDOWMINWIDTH) {
        rect.width = SEQWINDOWMINWIDTH;
    }
    if (rect.height < SEQWINDOWMINHEIGHT) {
        rect.height = SEQWINDOWMINHEIGHT;
    }
    if (rect.height != currentPos.height || rect.width != currentPos.width) {
        UnloadRenderTexture(windowTex);
        isWindowResized = true;
        windowTex = LoadRenderTexture(rect.width, rect.height);
    }
    currentPos = rect;
}

void Sequence::Initialize(Vector2 dims) { 
    name = "default";
    isWindowShown = true;
    windowTex = LoadRenderTexture(dims.x, dims.y);
    currentPos = Rectangle {.x = 50, .y = 50, .width = dims.x, .height = dims.y};
}
void Sequence::DrawWindowContent() {
    //defined in derived classes
    //note isWindowResized for use here

    //TODO: Have a default implementation here
    //Use a list of params that come from a certain selected sequence sample by the main gui sequence draw
    //Each param has a name and a float value, vector of std::tuple probably
    //params are loaded from file with sequence samples
}
Rectangle Sequence::GetCurrentPos() {
    return currentPos;
}

bool Intersects(Rectangle rect, Vector2 vec) {
    return (vec.x >= rect.x && vec.x < rect.x + rect.width &&
        vec.y >= rect.y && vec.y < rect.y + rect.height);
}

#ifndef RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
#define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24
#endif

void Sequence::Update() { 
    if (isWindowShown) {
        Vector2 mousePos = GetMousePosition();
        bool intersectsTop = false;
        bool intersectsBottom = false;
        bool intersectsLeft = false;
        bool intersectsRight = false;
        bool intersectsHeader = false;
        Rectangle currentRect = currentPos;
        if (Intersects({currentRect.x - SEQWINDOWINTERACTWIDTH, currentRect.y - SEQWINDOWINTERACTWIDTH - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT,
            SEQWINDOWINTERACTWIDTH, currentRect.height + SEQWINDOWINTERACTWIDTH * 2 + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT}, mousePos)) { intersectsLeft = true; }
        else if (Intersects({currentRect.x + currentRect.width, currentRect.y - SEQWINDOWINTERACTWIDTH - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT,
            SEQWINDOWINTERACTWIDTH, currentRect.height + SEQWINDOWINTERACTWIDTH * 2 + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT}, mousePos)) { intersectsRight = true; }
        if (Intersects({currentRect.x - SEQWINDOWINTERACTWIDTH, currentRect.y - SEQWINDOWINTERACTWIDTH - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT,
            currentRect.width + SEQWINDOWINTERACTWIDTH * 2, SEQWINDOWINTERACTWIDTH}, mousePos)) { intersectsTop = true; }
        else if (Intersects({currentRect.x - SEQWINDOWINTERACTWIDTH, currentRect.y + currentRect.height,
            currentRect.width + SEQWINDOWINTERACTWIDTH * 2, SEQWINDOWINTERACTWIDTH}, mousePos)) { intersectsBottom = true; }
        else if (Intersects({currentRect.x, currentRect.y - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, currentRect.width, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT}, mousePos)) {
            intersectsHeader = true; }
            
        bool isMouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

        // LLM: Clear stale edge selection when mouse is released
        if (!isMouseDown && selectedEdge != Edge::None) {
            selectedEdge = Edge::None;
        }

        //if there was an edge selected and mouse is still down, link their positions
        if (selectedEdge != Edge::None && isMouseDown) {
            Vector2 delt = GetMouseDelta();
            //if is top edge
            if (selectedEdge == Edge::Top || selectedEdge == Edge::TopRight || selectedEdge == Edge::TopLeft) {
                if (currentRect.height - delt.y > SEQWINDOWMINHEIGHT) {
                    currentRect.y += delt.y;
                    currentRect.height -= delt.y;
                }
                else {
                    currentRect.height = SEQWINDOWMINHEIGHT;
                }
            }
            //if is bottom edge
            else if (selectedEdge == Edge::Bottom || selectedEdge == Edge::BottomRight || selectedEdge == Edge::BottomLeft) {
                currentRect.height += delt.y;
            }
            //if is left edge
            if (selectedEdge == Edge::Left || selectedEdge == Edge::TopLeft || selectedEdge == Edge::BottomLeft) {
                if (currentRect.width - delt.x > SEQWINDOWMINWIDTH) {
                    currentRect.x += delt.x;
                    currentRect.width -= delt.x;
                }
                else {
                    currentRect.width = SEQWINDOWMINWIDTH;
                }
            }
            //if is right edge
            else if (selectedEdge == Edge::Right || selectedEdge == Edge::TopRight || selectedEdge == Edge::BottomRight) {
                currentRect.width += delt.x;
            }
            //if the window header is selected
            else if (selectedEdge == Edge::Header) {
                currentRect.x += delt.x;
                currentRect.y += delt.y;
                SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
            }

            UpdateCurrentPos(currentRect);
        }
        else if (intersectsTop || intersectsBottom || intersectsLeft || intersectsRight || intersectsHeader) {
            wasIntersecting = true;
            if (intersectsTop) {
                if (intersectsRight) {
                    SetMouseCursor(MOUSE_CURSOR_RESIZE_NESW);
                    if (isMouseDown) { selectedEdge = Edge::TopRight; }
                }
                else if (intersectsLeft) {
                    SetMouseCursor(MOUSE_CURSOR_RESIZE_NWSE);
                    if (isMouseDown) { selectedEdge = Edge::TopLeft; }
                }
                else {
                    SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
                    if (isMouseDown) { selectedEdge = Edge::Top; }
                }
            } 
            else if (intersectsBottom) {
                if (intersectsRight) {
                    SetMouseCursor(MOUSE_CURSOR_RESIZE_NWSE);
                    if (isMouseDown) { selectedEdge = Edge::BottomRight; }
                }
                else if (intersectsLeft) {
                    SetMouseCursor(MOUSE_CURSOR_RESIZE_NESW);
                    if (isMouseDown) { selectedEdge = Edge::BottomLeft; }
                }
                else {
                    SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
                    if (isMouseDown) { selectedEdge = Edge::Bottom; }
                }
            }
            else if (intersectsLeft) {
                SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
                if (isMouseDown) { selectedEdge = Edge::Left; }
            }
            else if (intersectsRight) {
                SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
                if (isMouseDown) { selectedEdge = Edge::Right; }
            }
            else if (intersectsHeader) {
                SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
                if (isMouseDown) { selectedEdge = Edge::Header; }
            }
        }
        else if (wasIntersecting) {
            wasIntersecting = false;
            if (!isMouseDown) {
                selectedEdge = Edge::None;
            }
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
    }
}

void Sequence::Draw() {
    if (isWindowShown) {
        if (DrawWindowBoxAround(currentPos, name)) {
            isWindowShown = false;
            AddToBottomBar(this);
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
        BeginTextureMode(windowTex);

        ClearBackground(GREEN);
        DrawWindowContent();

        EndTextureMode();
        DrawTexture(windowTex.texture, currentPos.x, currentPos.y, WHITE);
    }
}

Sequence::Sequence() {
    
}

void Sequence::LoadSong(std::string songPath) {
    LoadSequenceSamples(songPath + name + "/" + name + "seq.txt");
}

void Sequence::LoadSequenceSamples(std::string filePath) {
    //read from the file and add samples using virtual funcs

    std::ifstream inputStream(filePath);
    int currentLine = 0;
    if (inputStream.is_open()) {
        std::string lineStr;
        int currentMeasure = 0;
        while (!inputStream.eof()) {
            std::getline(inputStream, lineStr);
            currentLine++;
            if (lineStr.substr(0,2) != "//") {
                std::stringstream linestream(lineStr);
                if (linestream.peek() == 'M') {
                    currentMeasure++;
                    linestream.ignore(3, '-');
                    Measure measure;
                    linestream >> measure.bpm;
                    linestream >> measure.numerator;
                    linestream.ignore(1, '/');
                    linestream >> measure.denominator;
                    
                    measure.length = (60 / measure.bpm) * measure.numerator * (4.0f / measure.denominator);;
                    AddMeasureToCount(measure);

                }
                else {
                    float startBeat;
                    int repetitions = 1;
                    float beatGap = 0;
                    std::unordered_map<std::string, float> properties;
                    if (!(linestream >> startBeat)) {
                        std::cout << "issue loading startbeat from file at line " << currentLine << "\n";
                    }
                    if (linestream.peek() == ',') {
                        linestream.ignore(1); // skip comma
                        if (!(linestream >> beatGap)) {
                            std::cout << "issue loading beatGap from file at line " << currentLine << "\n";
                        }
                        linestream >> std::ws; // skip whitespace
                        if (linestream.peek() == 'x') {
                            linestream.ignore(1); // skip 'x'
                            if (!(linestream >> repetitions)) {
                                std::cout << "issue loading repetitions from file at line " << currentLine << "\n";
                            }
                        }
                    }
                    while (linestream.good()) {
                        linestream.ignore(std::numeric_limits<std::streamsize>::max(), '<');
                        if (!linestream.good()) break;
                        properties.clear();  // reset for each chord
                        std::string paramName;
                        float paramValue = 0;
                        while (linestream.good() && linestream.peek() != '>' && linestream.peek() != EOF) {
                            if (!(linestream >> paramName)) {
                                std::cout << "issue loading paramName from file at line " << currentLine << "\n";
                                break;
                            }
                            if (!(linestream >> paramValue)) {
                                std::cout << "issue loading paramValue from file at line " << currentLine << "\n";
                                break;
                            }
                            properties.emplace(paramName, paramValue);
                            if (linestream.peek() == ',') {
                                linestream.ignore(1);
                            }
                        }
                        if (linestream.peek() == '>') {
                            linestream.ignore(1);
                        }
                        Measure mes = std::get<0>(measures[currentMeasure - 1]);

                        if (properties.contains("len")) {
                            properties["len"] *= (60 / mes.bpm) * (4.0f / mes.denominator);
                        }
                        AddSamples(properties, GetBeatTime(currentMeasure, startBeat), repetitions, 
                        beatGap * (60 / mes.bpm) * (4.0f / mes.denominator));
                    }
                }
            }
        }
        inputStream.close();
        SortSamplesToAdd();
    }
    else {
        std::cout << "Failed to open file: " << filePath;
    }
}

void Sequence::SortSamplesToAdd() {
    std::sort(samplesToAdd.begin(), samplesToAdd.end(), 
    [](const SequenceSample& a, const SequenceSample& b) {
        return a.startTime < b.startTime;
    });
}

float Sequence::GetBeatTime(int measure, float beat) {
    if (measure > measures.size() || measure < 1) {
        throw std::invalid_argument("Can't get beat time of a measure out of bounds");
    }
    Measure measureobj = std::get<0>(measures.at(measure - 1));
    float time = std::get<1>(measures.at(measure - 1));
    time += beat * (60 / measureobj.bpm) * (4.0f / measureobj.denominator);
    return time;
}
void Sequence::AddMeasureToCount(Measure measure) {
    measures.push_back(std::tuple(measure, -1));
    UpdateMeasureTimes();
}
void Sequence::UpdateMeasureTimes() {
    float countedTime = 0;
    for (auto &element : measures) {
        Measure measure = std::get<0>(element);
        float time = countedTime;
        countedTime += measure.length;
        element = std::tuple(measure, time);
    }
}



float prevTime;
float Sequence::GetSampleAtTime(float time) {

    //check if sample vectors are valid if time is not progressing normally
    if (prevTime > time) {
        for (auto x = samplesAdded.end(); x != samplesAdded.begin();) {
            --x;
            SequenceSample samp = *x;
            //if the sample hasnt started yet at new time
            if (samp.startTime > time) {
                samplesToAdd.insert(samplesToAdd.begin(), samp);
                x = samplesAdded.erase(x);
                continue;
            }
            //if time is in the active period of the sample
            else if (samp.startTime + samp.sample->length >= time) {
                activeSamples.push_back(samp);
                x = samplesAdded.erase(x);
                continue;
            }
            //otherwise leave it in the past played samples
        }
    }
    else if (time > prevTime + 1 / SAMPLERATE - 0.0001) {
        //TODO: handle this
    }
    prevTime = time;


    while (!samplesToAdd.empty() && samplesToAdd.front().startTime <= time) {
        activeSamples.push_back(samplesToAdd.front());
        samplesAdded.push_back(samplesToAdd.front());
        samplesToAdd.erase(samplesToAdd.begin()); //pop front
    }
    
    float result = 0.0f;
    int sampleCount = 0;
    for (auto i = activeSamples.begin(); i != activeSamples.end(); ) {
        SequenceSample seqSample = *i;
        auto sample = seqSample.sample;
        if (time >= seqSample.startTime && time <= sample->length + seqSample.startTime) {
            result += sample->GetSample(time - seqSample.startTime) * sample->volumeMult;
            sampleCount++;
            ++i;
        }
        else if (time > sample->length + seqSample.startTime) {
            i = activeSamples.erase(i);
        }
        else {
            ++i;
        }
    }
    return sampleCount > 0 ? result / 3 : 0.0f;
}

void Sequence::AddSamples(std::unordered_map<std::string, float>, float startTime, int repetitions, float timeGap) {
    //should be defined in derived classes
}
// void Sequence::AddSamplesOfLength(std::shared_ptr<Sample> sample, float startTime, float freqMult, float length, int repetitions, float timeGap) {
//     for (int i = 0; i < repetitions; i++) {
//         SequenceSample sequenceSample { sample, startTime + timeGap * i, freqMult};
//         sequenceSample.sample.get()->length = length;
//         activeSamples.push_back(sequenceSample);
//     }
// }