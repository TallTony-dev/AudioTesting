#pragma once
#include "../include/sample.hpp"
#include <vector>

class Goop : public Sample {
    public:
        Goop(std::unordered_map<std::string, float> properties);
        void ApplyProperties(std::unordered_map<std::string, float> properties) override;
        float GetSample(float time) override;
    private:
        float freq;
};