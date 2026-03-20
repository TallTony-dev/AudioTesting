#pragma once
#include "../include/sequence.hpp"
#include "template.hpp"
#include "../include/plugininterface.hpp"
#include "../rayinclude/raylib.h"
#include <unordered_map>

class TemplateSequence : public Sequence {
    public:
        float GetSampleAtTime(float time) override;
        SequenceSample *AddSamples(std::unordered_map<std::string, SampleProperty> , float startTime, int repetitions = 1, float timeGap = 0) override;
        void Update() override; //called each loop to update any logic in samples
        void Initialize(Vector2 dims) override;
        void DrawWindowContent() override;
        void LoadSequenceSamples(std::string filePath) override;
};
