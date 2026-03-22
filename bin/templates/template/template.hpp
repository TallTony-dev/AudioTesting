#pragma once
#include "../include/sample.hpp"
#include <vector>

class Template : public Sample {
    public:
        Template(std::unordered_map<std::string, SampleProperty> properties);
        void ApplyProperties() override;
        float GetSample(double time) override;
};