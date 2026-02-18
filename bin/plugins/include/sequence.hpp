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
        virtual float GetSampleAtTime(float time);
        virtual void AddSamples(std::vector<float> params, float startTime, float freq, int repetitions = 1, float timeGap = 0);
        /// @brief called each loop to update any logic in samples and to draw things
        /// @param tex texture to draw anything that the plugin wants, on a user resizeable window with a border
        virtual void Update();
        virtual void Initialize(Vector2 dims);
        virtual void Deinitialize(); //free unmanaged stuff
        Rectangle texturePos; //where the actual window is drawn on screen
        RenderTexture2D tex; //texture that is drawn to screen
        bool isResized; //true if window was resized this frame
        bool isWindowShown; //true if close button hasnt been pressed
    protected:
        int currentMeasure = 1;
        float measureTimeTotal;
        void AddMeasureToCount(float currentTime);
        Sample *sampleType;
        std::vector<SequenceSample> activeSamples;
        std::vector<SequenceSample> samplesToAdd;
        std::vector<SequenceSample> samplesAdded;
};