#pragma once
#include "../sample.hpp"

class KickDrum : public Sample {
    public:
        KickDrum();
        float GetSample(float time, float freqMult);

};