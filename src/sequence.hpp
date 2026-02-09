#pragma once
#include <vector>
#include <memory>
#include <queue>
#include "instruments/sample.hpp"

typedef struct sequenceSample {
    std::shared_ptr<Sample> sample;
    float startTime;
    float freqMult;
} SequenceSample;


class Sequence {
    public:
        void Initialize(std::string relFilePath); //parse file
        void AddSamples(std::shared_ptr<Sample> sample, float startTime, float freqMult, int repetitions = 1, float timeGap = 0);
        //void Sequence::AddSamplesOfLength(std::shared_ptr<Sample> sample, float startTime, float freqMult, float length, int repetitions = 1, float timeGap = 0);
        float GetSampleAtTime(float time);
    private:
        int currentMeasure = 1;
        float measureTimeTotal;
        void AddMeasureToCount(float currentTime);
        std::vector<SequenceSample> activeSamples;
        std::queue<SequenceSample> samplesToAdd;
};