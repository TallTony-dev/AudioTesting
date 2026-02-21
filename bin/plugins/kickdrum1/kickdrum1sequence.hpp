#pragma once
#include "../include/sequence.hpp"
#include "kickdrum1.hpp"
#include "../include/plugininterface.hpp"
#include "../rayinclude/raylib.h"
#include <unordered_map>

class KickDrum1Sequence : public Sequence {
    public:
        float GetSampleAtTime(float time) override;
        void AddSamples(std::unordered_map<std::string, float> , float startTime, int repetitions = 1, float timeGap = 0) override;
        void Update() override; //called each loop to update any logic in samples
        void Initialize(Vector2 dims) override;
        void DrawWindowContent() override;
        void LoadSequenceSamples(std::string filePath) override;
};
