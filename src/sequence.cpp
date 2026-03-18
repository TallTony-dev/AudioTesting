#include "plugins/include/sequence.hpp"
#include "plugins/include/sample.hpp"
#include "raygui/maingui.hpp"
#include "raygui/raygui.h"
#include "plugins/include/helpers.hpp"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <ranges>
#include <string_view>
#include <string>
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <exception>
#include "playback.hpp"

#ifndef RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
#define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24
#endif

Sequence::~Sequence() {
    for (SequenceSample *samp : activeSamples) {
        delete (*samp).sample;
        delete samp;
    }
    for (SequenceSample *samp : samplesToAdd) {
        delete (*samp).sample;
        delete samp;
    }
    for (SequenceSample *samp : samplesAdded) {
        delete (*samp).sample;
        delete samp;
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
    Vector2 win = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    if (rect.x + rect.width / 4 > win.x) {
        rect.x = win.x - rect.width / 4;
    }
    else if (rect.x < -rect.width * 3 / 4) {
        rect.x = -rect.width * 3 / 4;
    }
    if (rect.y + rect.height / 4 > win.y) {
        rect.y = win.y - rect.height / 4;
    }
    else if (rect.y < RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT) {
        rect.y = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
    }
    currentPos = rect;
}
void Sequence::Initialize(Vector2 dims) { 
    name = "default";
    isWindowShown = true;
    windowTex = LoadRenderTexture(dims.x, dims.y);
    AddToSequenceBar(this);
    currentPos = Rectangle {.x = 50, .y = 50, .width = dims.x, .height = dims.y};
    seqHeight = 60;
}
void Sequence::DrawWindowContent() {
    //defined in derived classes
    //note isWindowResized for use here
    if (selectedSamp != nullptr) {
        bool hasPropertyChanges = false;
        int count = 0;
        for (auto &[name, prop] : selectedSamp->sample->properties) {

            float height = 60;
            float width = 60;
            float x = 0;
            float y = count * height;
            float priorValue = prop.val;
            GuiSliderBar({x,y,width,height}, std::to_string(prop.min).c_str(), std::to_string(prop.max).c_str()
            , &prop.val, prop.min, prop.max);
            if (prop.val != priorValue) {
                hasPropertyChanges = true;
            }

            count++;
        }
        if (hasPropertyChanges) {
            selectedSamp->sample->ApplyProperties();
        }
    }
}
std::vector<SequenceSample*> Sequence::GetAllSamples() {
    std::vector<SequenceSample *> samps = samplesAdded;
    samps.insert(samps.end(), samplesToAdd.begin(), samplesToAdd.end());
    return samps;
}
std::vector<std::tuple<Measure, float>> Sequence::GetMeasures() {
    return measures;
}

Rectangle Sequence::GetCurrentPos() {
    return currentPos;
}
Rectangle Sequence::GetCurrentWindowPos() {
    Rectangle rec = currentPos;
    rec.y -= RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
    rec.height += RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
    return rec;
}
Rectangle Sequence::GetCurrentWindowPaddedPos() {
    Rectangle rec = currentPos;
    rec.x -= SEQWINDOWINTERACTWIDTH;
    rec.width += SEQWINDOWINTERACTWIDTH * 2;
    rec.y -= SEQWINDOWINTERACTWIDTH + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
    rec.height += SEQWINDOWINTERACTWIDTH * 2 + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
    return rec;
}

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
        bool isMousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        // LLM: Clear stale edge selection when mouse is released
        if (!isMouseDown && selectedEdge != Edge::None) {
            selectedEdge = Edge::None;
        }

        //if the window is selected or should be selected
        if (IsWindowSelected(this)) { 
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
                wasWindowIntersecting = true;
                if (intersectsTop) {
                    if (intersectsRight) {
                        SetMouseCursor(MOUSE_CURSOR_RESIZE_NESW);
                        if (isMousePressed) { selectedEdge = Edge::TopRight; }
                    }
                    else if (intersectsLeft) {
                        SetMouseCursor(MOUSE_CURSOR_RESIZE_NWSE);
                        if (isMousePressed) { selectedEdge = Edge::TopLeft; }
                    }
                    else {
                        SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
                        if (isMousePressed) { selectedEdge = Edge::Top; }
                    }
                } 
                else if (intersectsBottom) {
                    if (intersectsRight) {
                        SetMouseCursor(MOUSE_CURSOR_RESIZE_NWSE);
                        if (isMousePressed) { selectedEdge = Edge::BottomRight; }
                    }
                    else if (intersectsLeft) {
                        SetMouseCursor(MOUSE_CURSOR_RESIZE_NESW);
                        if (isMousePressed) { selectedEdge = Edge::BottomLeft; }
                    }
                    else {
                        SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
                        if (isMousePressed) { selectedEdge = Edge::Bottom; }
                    }
                }
                else if (intersectsLeft) {
                    SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
                    if (isMousePressed) { selectedEdge = Edge::Left; }
                }
                else if (intersectsRight) {
                    SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
                    if (isMousePressed) { selectedEdge = Edge::Right; }
                }
                else if (intersectsHeader) {
                    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
                    if (isMousePressed) { selectedEdge = Edge::Header; }
                }
            }
            else if (wasWindowIntersecting) {
                wasWindowIntersecting = false;
                if (!isMouseDown) {
                    selectedEdge = Edge::None;
                }
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }
        }
        //if isnt yet selected
        else {
            
        }
    }
    
}

void Sequence::DrawWindow() {
    if (isWindowShown) {
        if (CreateWindowBoxAround(currentPos, name)) {
            isWindowShown = false;
            AddToBottomBar(this);
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
        BeginTextureMode(windowTex);

        ClearBackground(GREEN);

        SetMouseOffset(-currentPos.x,-currentPos.y);
        DrawWindowContent();
        SetMouseOffset(0,0);

        EndTextureMode();
        DrawTextureRec(windowTex.texture, {0,0,(float)windowTex.texture.width,-(float)windowTex.texture.height}, {currentPos.x, currentPos.y}, WHITE);
    }
}

Sequence::Sequence() {
    
}

void Sequence::LoadSong(std::string songPath) {

    for (SequenceSample *samp : GetAllSamples()) {
        delete samp->sample;
        delete samp;
    }

    activeSamples.clear();
    samplesToAdd.clear();
    samplesAdded.clear();
    measures.clear();
    lastDrawnSamples.clear();
    selectedSamp = nullptr;
    highlightedSamp = nullptr;
    prevTime = 0.0f;

    LoadSequenceSamples(songPath + name + "/" + name + "seq.txt");
    loadedSongPath = songPath;
}

float Sequence::GetBeatAtTime(float time) {
    std::tuple<Measure, float> *measureAtTime = GetMeasureAtTime(time);
    if (measureAtTime == nullptr) {
        throw std::invalid_argument("No measure found for requested time");
    }
    std::tuple<Measure, float> tup = *measureAtTime;
    Measure mes = std::get<0>(tup);
    float startTime = std::get<1>(tup);
    time -= startTime;
    float beat = time / ((60 / mes.bpm) * (4.0f / mes.denominator));
    return beat;
}
void Sequence::SaveLoadedSong() {
    SaveSong(loadedSongPath);
}
void Sequence::SaveSong(std::string songPath) {
    std::string pathish = songPath + name + "/" + name;
    std::ofstream ostream(pathish + "seqtemp.txt");
    if (!ostream.is_open()) {    
        throw std::invalid_argument("Error creating the file/output stream, possibly invalid path");
    }

    std::vector<SequenceSample *> samps = GetAllSamples();
    std::sort(samps.begin(), samps.end(), 
    [](const SequenceSample* a, const SequenceSample* b) {
        return (*a).startTime < (*b).startTime;
    });

    if (samps.empty()) {
        ostream.close();
        std::filesystem::rename(pathish + "seqtemp.txt", pathish + "seq.txt");
        return;
    }
    if (measures.empty()) {
        throw std::invalid_argument("Cannot save sequence with no measures");
    }

    size_t currentMeasure = 0;
    Measure mes = std::get<0>(measures[currentMeasure]);
    ostream << std::format("M - {} {}/{}", mes.bpm, mes.numerator, mes.denominator) << std::endl;

    for (auto iter = samps.begin(); iter != samps.end();) {
        SequenceSample *samp = *iter;
        while (currentMeasure + 1 < measures.size() && std::get<1>(measures[currentMeasure + 1]) <= samp->startTime) {
            currentMeasure++;
            mes = std::get<0>(measures[currentMeasure]);
            //serialize measure into ostream here
            ostream << std::format("M - {} {}/{}", mes.bpm, mes.numerator, mes.denominator) << std::endl;
        }
        //serialize samples into ostream here
        float beat = GetBeatAtTime(samp->startTime);
        if (std::abs(beat) < 0.0001f) {
            beat = 0.0f;
        }
        float beatLen = (60 / mes.bpm) * (4.0f / mes.denominator);
        ostream << std::format("{}, 0 x 1 ", beat);

        do {
            samp = *iter;
            ostream << "<";
            for (auto &[name, prop] : samp->sample->properties) {
                float writeVal = prop.val;
                if (name == "len" && beatLen > 0.0f) {
                    writeVal = prop.val / beatLen;
                }
                ostream << name << " " << writeVal << ",";
            }
            ostream << "> ";
        } while ((++iter != samps.end()) && ((*(iter - 1))->startTime == (*iter)->startTime));
        if (iter != samps.end()) { ostream << std::endl; }
    }
    ostream.close();
    std::filesystem::rename(pathish + "seqtemp.txt", pathish + "seq.txt");
    //std::filesystem::remove(pathish + "seqtemp.txt");
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
            if (lineStr.compare(0, 2, "//") != 0) {
                std::stringstream linestream(lineStr);
                if (linestream.peek() == 'M') {
                    currentMeasure++;
                    linestream.ignore(3, '-');
                    Measure measure;
                    linestream >> measure.bpm;
                    linestream >> measure.numerator;
                    linestream.ignore(1, '/');
                    linestream >> measure.denominator;
                    
                    measure.length = (60 / measure.bpm) * measure.numerator * (4.0f / measure.denominator);
                    AddMeasureToCount(measure);

                }
                else {
                    float startBeat;
                    int repetitions = 1;
                    float beatGap = 0;
                    std::unordered_map<std::string, SampleProperty> properties;
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
                        SampleProperty prop{};
                        while (linestream.good() && linestream.peek() != '>' && linestream.peek() != EOF) {
                            if (!(linestream >> paramName)) {
                                std::cout << "issue loading paramName from file at line " << currentLine << "\n";
                                break;
                            }
                            if (!(linestream >> paramValue)) {
                                std::cout << "issue loading paramValue from file at line " << currentLine << "\n";
                                break;
                            }
                            prop.val = paramValue;
                            properties.emplace(paramName, prop);
                            if (linestream.peek() == ',') {
                                linestream.ignore(1);
                            }
                        }
                        if (linestream.peek() == '>') {
                            linestream.ignore(1);
                        }
                        Measure mes = std::get<0>(measures[currentMeasure - 1]);

                        if (properties.contains("len")) {
                            properties["len"].val *= (60 / mes.bpm) * (4.0f / mes.denominator);
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
void Sequence::AddSequenceSample(SequenceSample *seqSamp, float startTime) {
    if (startTime > currentTime) {
        samplesToAdd.push_back(seqSamp);
        SortSamplesToAdd();
    }
    else {
        activeSamples.push_back(seqSamp);
        samplesAdded.push_back(seqSamp);
        SortSamplesAdded();
    }
}
void Sequence::SortSamplesToAdd() {
    std::sort(samplesToAdd.begin(), samplesToAdd.end(), 
    [](const SequenceSample* a, const SequenceSample* b) {
        return (*a).startTime < (*b).startTime;
    });
}
void Sequence::SortSamplesAdded() {
    std::sort(samplesAdded.begin(), samplesAdded.end(), 
    [](const SequenceSample* a, const SequenceSample* b) {
        return (*a).startTime < (*b).startTime;
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
    //check if sample vectors are valid if gone back in time
    if (prevTime > time) {
        for (auto x = samplesAdded.end(); x != samplesAdded.begin();) {
            //add samples back from samplesadded into active 
            --x;
            SequenceSample *samp = *x;
            //if the sample hasnt started yet at new time
            if ((*samp).startTime > time) {
                samplesToAdd.insert(samplesToAdd.begin(), samp);
                x = samplesAdded.erase(x);
                std::erase(activeSamples, samp);
                continue;
            }
            //if time is in the active period of the sample and it isnt already active
            else if ((*samp).startTime + (*samp).sample->length >= time
            && activeSamples.end() == std::find(activeSamples.begin(), activeSamples.end(), samp)){
                activeSamples.push_back(samp);
                continue;
            }
            //otherwise leave it in the past played samples
        }
    }
    //check if sample vectors are valid if jumped forward
    else if (time > prevTime + 1 / SAMPLERATE - 0.00001) {
        //TODO: handle this
        

    }
    prevTime = time;


    while (!samplesToAdd.empty() && (*samplesToAdd.front()).startTime <= time) {
        activeSamples.push_back(samplesToAdd.front());
        samplesAdded.push_back(samplesToAdd.front());
        samplesToAdd.erase(samplesToAdd.begin()); //pop front
    }
    
    float result = 0.0f;
    int sampleCount = 0;
    for (auto i = activeSamples.begin(); i != activeSamples.end(); ) {
        SequenceSample *seqSample = *i;
        auto sample = (*seqSample).sample;
        if (time >= (*seqSample).startTime && time <= sample->length + (*seqSample).startTime) {
            result += sample->GetSample(time - (*seqSample).startTime) * sample->volumeMult;
            sampleCount++;
            ++i;
        }
        else if (time > sample->length + (*seqSample).startTime) {
            i = activeSamples.erase(i);
        }
        else {
            ++i;
        }
    }
    return sampleCount > 0 ? result / 3 : 0.0f;
}

std::tuple<Measure, float> *Sequence::GetMeasureAtTime(float time) {
    for (auto &meas : measures) {
        Measure measure = std::get<0>(meas);
        float startTime = std::get<1>(meas);
        if (time >= startTime && time < startTime + measure.length) {
            return &meas;
        }
    }
    return nullptr;
}

SequenceSample *Sequence::AddSamples(std::unordered_map<std::string, SampleProperty>, float startTime, int repetitions, float timeGap) {
    //should be defined in derived classesf
    return nullptr;
}
