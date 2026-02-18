#include "plugins/include/sequence.hpp"
#include "plugins/include/sample.hpp"
#include "include/raylib.h"
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

void Sequence::Initialize(Vector2 dims) { 
    tex = LoadRenderTexture(dims.x, dims.y); //there should be a rendertex associated with each sequence along with location info
    texturePos = Rectangle {.x = 50, .y = 50, .width = dims.x, .height = dims.y};
    isWindowShown = true;
}
void Sequence::Update() { 
    if (isResized) {
        UnloadRenderTexture(tex);
        tex = LoadRenderTexture(texturePos.width, texturePos.height); //there should be a rendertex associated with each sequence along with location info
    }
}
void Sequence::Deinitialize() { 
    UnloadRenderTexture(tex);
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

// float prevTime;
float Sequence::GetSampleAtTime(float time) {
    float result = 0.0f;
    int sampleCount = 0;
    
    //Currently going back in time isn't supported
    // if (prevTime > time) {
    //     for (auto x = samplesAdded.end(); x != samplesAdded.begin(); x++) {
    //         samplesToAdd.insert(samplesToAdd.begin(), *x); //push old ones to front
    //     }
    // }
    // prevTime = time;


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

}
// void Sequence::AddSamplesOfLength(std::shared_ptr<Sample> sample, float startTime, float freqMult, float length, int repetitions, float timeGap) {
//     for (int i = 0; i < repetitions; i++) {
//         SequenceSample sequenceSample { sample, startTime + timeGap * i, freqMult};
//         sequenceSample.sample.get()->length = length;
//         activeSamples.push_back(sequenceSample);
//     }
// }