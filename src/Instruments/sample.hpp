#pragma once

class Sample {
    public:
        virtual float GetSample(float time, float freq); //returns 0-1
        float length;
        float volumeMult;
};