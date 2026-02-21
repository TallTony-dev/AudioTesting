#include "kickdrum1.hpp"
#include "../include/helpers.hpp"
#include <unordered_map>

KickDrum::KickDrum(std::unordered_map<std::string, float> properties) {
    length = 0.5;
    volumeMult = 1;
    ApplyProperties(properties);    
}
void KickDrum::ApplyProperties(std::unordered_map<std::string, float> properties) {
    if (properties.contains("len")) {
        length = properties["len"];
    }
    if (properties.contains("vol")) {
        volumeMult = properties["vol"];
    }
    if (properties.contains("freq")) {
        freq = properties["freq"];
    }
}

float KickDrum::GetSample(float time) {
    float amp = GETSIN(freq / (time * 5 + 1), 1 / (10 * time * time + 1), time, 0);

    if (length - time < 0.05) {
        amp *= 1.0f - (time - length + 0.05) / 0.05;
    }

    return amp;
}
