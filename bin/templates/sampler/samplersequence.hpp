#pragma once
#include "../include/sequence.hpp"
#include "sampler.hpp"
#include "../include/plugininterface.hpp"
#include "../rayinclude/raylib.h"
#include <unordered_map>

class SamplerSequence : public Sequence {
    public:
        float GetSampleAtTime(double time) override;
        SequenceSample *AddSamples(std::unordered_map<std::string, SampleProperty> , double startTime, int repetitions = 1, double timeGap = 0) override;
        void Update() override; //called each loop to update any logic in samples
        void Initialize(Vector2 dims) override;
        void DrawWindowContent() override;
        void LoadSequenceSamples(std::string filePath) override;
        void LoadSong(std::string songPath) override;
        ~SamplerSequence() override;
        inline int GetSafeIndexAtTime(float time);
        float GetSampleAtTime(float time);
        int sampLength;
    protected:
        float *buf = nullptr;
        int bufLength = 0;
        int sampleRate = 0;
};
