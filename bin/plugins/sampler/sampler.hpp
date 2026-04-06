#pragma once
#include "../include/sample.hpp"
#include <string>
#include <unordered_map>

class SamplerSequence;

class Sampler : public Sample {
    public:
        Sampler(std::unordered_map<std::string, SampleProperty> properties, SamplerSequence *owner);
        void ApplyProperties() override;
        float GetSample(double time) override;
    private:
        SamplerSequence *_owner;
};