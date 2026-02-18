#pragma once
#include <vector>
#include <memory>
#include <fstream>
#include <queue>
#include "sample.hpp"
#include "../rayinclude/raylib.h"

typedef struct sequenceSample {
    Sample *sample;
    float startTime;
    float freqMult;
} SequenceSample;


class Sequence {
    public:
        Sequence(std::string relFilePath);
        Sequence(); //empty
        virtual ~Sequence(); //destructor
        //void Sequence::AddSamplesOfLength(std::shared_ptr<Sample> sample, float startTime, float freqMult, float length, int repetitions = 1, float timeGap = 0);
        virtual float GetSampleAtTime(float time); //updates sample logic based off time and gets sample, should be in range 0,1
        virtual void AddSamples(std::vector<float> params, float startTime, float freq, int repetitions = 1, float timeGap = 0);
        void Draw(); //starts draw to texture and calls update
        virtual void Update();
        virtual void Initialize(Vector2 dims);
        std::string name;
        bool isWindowShown; //true if close button hasnt been pressed
    protected:
        Rectangle currentPos;
        RenderTexture2D tex; 
        virtual void DrawContent(); //called after setting up drawing to window
        int currentMeasure = 1;
        float measureTimeTotal;
        void AddMeasureToCount(float currentTime);
        Sample *sampleType;
        std::vector<SequenceSample> activeSamples;
        std::vector<SequenceSample> samplesToAdd;
        std::vector<SequenceSample> samplesAdded;
    private:
        void UpdateWindowPos();
};