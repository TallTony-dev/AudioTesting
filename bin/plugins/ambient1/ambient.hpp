#pragma once
#include "../include/sample.hpp"
#include <vector>

class Ambient1 : public Sample {
    public:
        Ambient1(std::unordered_map<std::string, SampleProperty> properties);
        void ApplyProperties() override;
        float GetSample(float time) override;
};