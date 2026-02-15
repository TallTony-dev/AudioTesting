#pragma once
#include "../sample.hpp"

class GoopSynth : public Sample {
    public:
        GoopSynth();
        float attackMult;
        float attackLength;
        float GetSample(float time, float freq);

};