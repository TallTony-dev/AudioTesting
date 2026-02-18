#pragma once
#include "../sequence.hpp"
#include "kickdrum1.hpp"
#include "../plugininterface.hpp"
#include "../include/raylib.h"

class KickDrum1Sequence : public Sequence {
    public:
        float GetSampleAtTime(float time) override;
        void AddSamples(std::vector<float> params, float startTime, float freq, int repetitions = 1, float timeGap = 0) override;
        void Update() override; //called each loop to update any logic in samples
        void Initialize(Vector2 dims) override;
        void Deinitialize() override;
};
