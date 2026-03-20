#include "template.hpp"
#include "../include/helpers.hpp"
#include <unordered_map>

Template::Template(std::unordered_map<std::string, SampleProperty> _properties) {
    length = 0.5;
    volumeMult = 1;
    properties = _properties;  
    ApplyProperties();
}
void Template::ApplyProperties() {
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

float Template::GetSample(double time) {
    return GETSIN(300, 1, time, 0);
}
