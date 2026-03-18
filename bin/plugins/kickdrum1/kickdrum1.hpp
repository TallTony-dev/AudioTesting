#pragma once
#include "../include/sample.hpp"
#include <vector>

class KickDrum : public Sample {
    public:
        KickDrum(std::unordered_map<std::string, SampleProperty> properties);
        void ApplyProperties() override;
        float GetSample(float time) override;
};