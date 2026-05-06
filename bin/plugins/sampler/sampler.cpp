#include "sampler.hpp"
#include "../include/helpers.hpp"
#include "../include/maingui.hpp"
#include "samplersequence.hpp"
#include <unordered_map>

Sampler::Sampler(std::unordered_map<std::string, SampleProperty> _properties, SamplerSequence *owner) {
    length = 0.5;
    volumeMult = 1;
    properties = _properties;  
    _owner = owner;
    ApplyProperties();
}
void Sampler::ApplyProperties() {
    //if len is longer than end - start then it loops
    if (properties.contains("len")) {
        length = properties["len"].val;
        properties["len"].min = 0;
        properties["len"].max = _owner->sampLength;
    }
    //indicates the start of the sample time for chops
    if (properties.contains("start")) {
        properties["start"].min = 0;
        properties["start"].max = _owner->sampLength;
    }
    if (properties.contains("end")) {
        properties["end"].min = 0;
        properties["end"].max = _owner->sampLength;
    }
    if (properties.contains("vol")) {
        volumeMult = properties["vol"].val;
        properties["vol"].min = 0;
        properties["vol"].max = 1;
    }
}

Rectangle Sampler::DrawSample(float yOffset, float startTime, float seqHeight, bool isSelected, bool isHighlighted, bool hasfreq) {

    Rectangle rect = Sample::DrawSample(yOffset, startTime, seqHeight, isSelected, isHighlighted, false);

    if (_owner->buf != nullptr && rect.width > 0) {
        for (int p = 0; p < rect.width; p++) {
            double timeInSample = (p / rect.width) * length;
            double time = std::fmod(timeInSample, properties["end"].val - properties["start"].val);
            float sampVal = _owner->GetLoadedSampleAtTime(properties["start"].val + time);
            DrawRectangle(p + rect.x, sampVal * seqHeight / 2 + seqHeight / 2 + yOffset, 1, 2, YELLOW);
        }
    }

    return rect;
}


float Sampler::GetSample(double time) {
    time = std::fmod(time, properties["end"].val - properties["start"].val);
    return _owner->GetLoadedSampleAtTime(properties["start"].val + time) * volumeMult;
}
