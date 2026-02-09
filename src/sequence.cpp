#include "sequence.hpp"
#include "instruments/sample.hpp"



void Sequence::Initialize(std::string relFilePath) {
    //read from the file
    samplesToAdd.push(bleh);
}


void Sequence::AddMeasureToCount(float currentTime) {
    currentMeasure++;
    measureTimeTotal = currentTime;
}

float Sequence::GetSampleAtTime(float time) {
    float result = 0.0f;
    int sampleCount = 0;
    
    while (samplesToAdd.front().startTime > time) {
        activeSamples.push_back(samplesToAdd.front());
        samplesToAdd.pop();
    }

    for (auto i = activeSamples.begin(); i != activeSamples.end(); i++) {
        SequenceSample seqSample = *i;
        auto sample = seqSample.sample;
        if (time >= seqSample.startTime && time <= sample->length + seqSample.startTime) {
            result += sample->GetSample(time - seqSample.startTime, seqSample.freqMult) * sample->volumeMult;
            sampleCount++;
        }
        else if (time > sample->length + seqSample.startTime) {
            activeSamples.erase(i);
        }
    }
    return result / sampleCount;
}

void Sequence::AddSamples(std::shared_ptr<Sample> sample, float startTime, float freqMult, int repetitions, float timeGap) {
    for (int i = 0; i < repetitions; i++) {
        activeSamples.push_back(SequenceSample{ sample, startTime + timeGap * i, freqMult});
    }
}
// void Sequence::AddSamplesOfLength(std::shared_ptr<Sample> sample, float startTime, float freqMult, float length, int repetitions, float timeGap) {
//     for (int i = 0; i < repetitions; i++) {
//         SequenceSample sequenceSample { sample, startTime + timeGap * i, freqMult};
//         sequenceSample.sample.get()->length = length;
//         activeSamples.push_back(sequenceSample);
//     }
// }