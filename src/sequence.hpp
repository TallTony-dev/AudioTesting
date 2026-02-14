#pragma once
#include <vector>
#include <memory>
#include <fstream>
#include <queue>
#include "instruments/sample.hpp"

typedef struct sequenceSample {
    std::shared_ptr<Sample> sample;
    float startTime;
    float freqMult;
} SequenceSample;


class Sequence {
    public:
        Sequence(std::string relFilePath, Sample *sampleType);
        Sequence(); //empty
        //void Sequence::AddSamplesOfLength(std::shared_ptr<Sample> sample, float startTime, float freqMult, float length, int repetitions = 1, float timeGap = 0);
        virtual float GetSampleAtTime(float time);
        virtual void AddSamples(std::shared_ptr<Sample> sample, float startTime, float freq, int repetitions = 1, float timeGap = 0);
        virtual void Update(); //called each loop to update any logic in samples
        virtual void Initialize();
        virtual void Deinitialize(); //free unmanaged stuff
    private:
        int currentMeasure = 1;
        float measureTimeTotal;
        void AddMeasureToCount(float currentTime);
        Sample *sampleType;
        std::vector<SequenceSample> activeSamples;
        std::vector<SequenceSample> samplesToAdd;
        std::vector<SequenceSample> samplesAdded;
};