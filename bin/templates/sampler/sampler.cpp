#include "sampler.hpp"
#include "../include/helpers.hpp"
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
        length = properties["start"].val;
        properties["start"].min = 0;
        properties["start"].max = _owner->sampLength;
    }
    if (properties.contains("end")) {
        length = properties["end"].val;
        properties["end"].min = 0;
        properties["end"].max = _owner->sampLength;
    }
    if (properties.contains("vol")) {
        volumeMult = properties["vol"].val;
        properties["vol"].min = 0;
        properties["vol"].max = 1;
    }
}

float Sampler::GetSample(double time) {
    return _owner->GetSampleAtTime(time) * volumeMult;
}
