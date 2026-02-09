#pragma once
#include "../sample.hpp"

class HiHat1 : public Sample {
    public:
        HiHat1();
        float GetSample(float time, float freqMult);
    private:
        float lastSample;
};