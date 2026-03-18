#pragma once
#include "../include/sample.hpp"
#include <vector>

class Goop : public Sample {
    public:
        Goop(std::unordered_map<std::string, SampleProperty> properties);
        void ApplyProperties() override;
        float GetSample(float time) override;
};