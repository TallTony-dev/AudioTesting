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
    if (tex.id != 0)
        UnloadRenderTexture(tex);
}   

void Sequence::UpdateWindowPos() {
    UnloadRenderTexture(tex);
    tex = LoadRenderTexture(currentPos.width, currentPos.height);
}

void Sequence::Initialize(Vector2 dims) { 
    name = "default";
    isWindowShown = true;
    tex = LoadRenderTexture(dims.x, dims.y);
    currentPos = Rectangle {.x = 50, .y = 50, .width = dims.x, .height = dims.y};
}
void Sequence::DrawContent() {
    //defined in derived classes
}
void Sequence::Update() { 
    // TODO: implement mouse handling for the sequence window
}

void Sequence::Draw() {
    if (isWindowShown) {
        if (DrawWindowBoxAround(currentPos, name)) {
            isWindowShown = false;
            AddToBottomBar(this);
        }
        BeginTextureMode(tex);

        ClearBackground(GREEN);
        DrawContent();

        EndTextureMode();
        DrawTexture(tex.texture, currentPos.x, currentPos.y, WHITE);
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