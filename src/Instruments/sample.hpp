#pragma once

class Sample {
    public:
        virtual float GetSample(float time, float freqMult); //returns 0-1
        float length;
        float volumeMult;
};