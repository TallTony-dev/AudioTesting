#include "plugins/include/sequence.hpp"
#include "plugins/include/sample.hpp"
#include "raygui/maingui.hpp"
#include "raygui/raygui.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <ranges>
#include <string_view>
#include <string>


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

Sequence::Sequence(std::string relFilePath) {
    //read from the file and add samples using virtual funcs

    std::ifstream inputStream(relFilePath);
    if (inputStream.is_open()) {
        std::string line;
        while (std::getline(inputStream, line)) {
            //process stuff here
            if (line.substr(0,2) != "//") {
                


                for (auto word : line | std::views::split(' ')) {
                    std::cout << std::string_view(word) << "\n";
                }
            }
        }
        inputStream.close();
    }
    else {
        std::cout << "Failed to open file: " << relFilePath;
    }
}


void Sequence::AddMeasureToCount(float currentTime) {
    currentMeasure++;
    measureTimeTotal = currentTime;
}

float prevTime;
float Sequence::GetSampleAtTime(float time) {
    float result = 0.0f;
    int sampleCount = 0;

    if (prevTime > time) {
        for (auto x = samplesAdded.end(); x != samplesAdded.begin();) {
            SequenceSample samp = *x;
            //if the sample hasnt started yet at new time
            if (samp.startTime > time) {
                samplesToAdd.insert(samplesToAdd.begin(), samp);
                samplesAdded.erase(x);
                continue;
            }
            //if time is in the active period of the sample
            else if (samp.startTime + samp.sample->length >= time) {
                activeSamples.push_back(samp);
                samplesAdded.erase(x);
                continue;
            }
            //otherwise leave it in the past played samples
            x++;
        }
    }
    prevTime = time;


    while (!samplesToAdd.empty() && samplesToAdd.front().startTime <= time) {
        activeSamples.push_back(samplesToAdd.front());
        samplesAdded.push_back(samplesToAdd.front());
        samplesToAdd.erase(samplesToAdd.begin()); //pop front
    }

    for (auto i = activeSamples.begin(); i != activeSamples.end(); ) {
        SequenceSample seqSample = *i;
        auto sample = seqSample.sample;
        if (time >= seqSample.startTime && time <= sample->length + seqSample.startTime) {
            result += sample->GetSample(time - seqSample.startTime, seqSample.freqMult) * sample->volumeMult;
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
    return sampleCount > 0 ? result / sampleCount : 0.0f;
}

void Sequence::AddSamples(std::vector<float> params, float startTime, float freq, int repetitions, float timeGap) {
    //should be defined in derived classes
}
// void Sequence::AddSamplesOfLength(std::shared_ptr<Sample> sample, float startTime, float freqMult, float length, int repetitions, float timeGap) {
//     for (int i = 0; i < repetitions; i++) {
//         SequenceSample sequenceSample { sample, startTime + timeGap * i, freqMult};
//         sequenceSample.sample.get()->length = length;
//         activeSamples.push_back(sequenceSample);
//     }
// }