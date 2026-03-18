#include "kickdrum1.hpp"
#include "../include/helpers.hpp"
#include <unordered_map>

KickDrum::KickDrum(std::unordered_map<std::string, SampleProperty> _properties) {
    length = 0.5;
    volumeMult = 1;
    properties = _properties;  
    ApplyProperties();
}
void KickDrum::ApplyProperties() {
    if (properties.contains("len")) {
        length = properties["len"].val;
        properties["len"].min = 0;
        properties["len"].max = 10;
    }
    if (properties.contains("vol")) {
        volumeMult = properties["vol"].val;
        properties["vol"].min = 0;
        properties["vol"].max = 1;
    }
    if (properties.contains("freq")) {
        freq = properties["freq"].val;
        properties["freq"].min = 25;
        properties["freq"].max = 500;
    }
}

float KickDrum::GetSample(float time) {
    float amp = GETSIN(freq / (time * 5 + 1), 1 / (10 * time * time + 1), time, 0);

    if (length - time < 0.05) {
        amp *= 1.0f - (time - length + 0.05) / 0.05;
    }

    return amp;
}
