#pragma once
#include <vector>
#include <unordered_map>

class Sample {
    public:
        virtual void ApplyProperties(std::unordered_map<std::string, float> properties);
        virtual float GetSample(float time); //returns 0-1
        float length;
        float volumeMult;
        float freq = -1; //init'd to -1 as not nessecary, and will display differently at -1
};