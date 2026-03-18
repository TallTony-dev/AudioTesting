#pragma once
#include <vector>
#include <unordered_map>

typedef struct sampleProperty {
    float val = 0.0f;
    float min = 0.0f;
    float max = 1.0f;
} SampleProperty;


class Sample {
    public:
        virtual void ApplyProperties();
        virtual float GetSample(float time); //returns 0-1
        std::unordered_map<std::string, SampleProperty> properties;
        float length;
        float volumeMult;
        float freq = -1; //init'd to -1 as not nessecary, and will display differently at -1
};