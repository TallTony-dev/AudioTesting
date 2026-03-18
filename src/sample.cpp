#include "plugins/include/sample.hpp"
#include <exception>

void Sample::ApplyProperties() {
    //defined in derived classes
    throw std::runtime_error("Base sample class init shouldn't be called");
}

float Sample::GetSample(float time) {
    throw std::runtime_error("Base sample class GetSample shouldn't be called");
    return 0;
}