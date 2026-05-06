#pragma once
#include <vector>
#include <unordered_map>

typedef struct sampleProperty {
    float val = 0.0f;
    float min = 0.0f;
    float max = 1.0f;
} SampleProperty;

struct Rectangle; //defined elsewhere

class Sample {
    public:
        virtual void ApplyProperties();
        virtual float GetSample(double time); //returns 0-1
        virtual Rectangle DrawSample(float yOffset, float startTime, float seqHeight, bool isSelected, bool isHighlighted, bool hasFreq = true);
        std::unordered_map<std::string, SampleProperty> properties;
        float length;
        float volumeMult;
        float freq = -1; //init'd to -1 as not nessecary, and will display differently at -1
};