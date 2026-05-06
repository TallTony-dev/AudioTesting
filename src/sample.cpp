#include "plugins/include/sample.hpp"
#include "plugins/include/maingui.hpp"
#include "raygui/raylib/raylib/include/raylib.h"
#include <exception>
#include <algorithm>

void Sample::ApplyProperties() {
    //defined in derived classes
    throw std::runtime_error("Base sample class init shouldn't be called");
}

float Sample::GetSample(double time) {
    throw std::runtime_error("Base sample class GetSample shouldn't be called");
    return 0;
}
Rectangle Sample::DrawSample(float yOffset, float startTime, float seqHeight, bool isSelected, bool isHighlighted, bool hasfreq) {
    float sampx = ConvertTimeToXPos(startTime);
    float sampWidth = ConvertTimeToXPos(startTime + length) - sampx;
    Color col = DARKGREEN;
    float sampHeight = hasfreq ? 3 : seqHeight;
    
    float sampy = yOffset + seqHeight - sampHeight - (hasfreq ? std::clamp((freq - 100) / 5, 0.0f, seqHeight) : 0);
    
    if (isSelected) {
        col = DARKPURPLE;
    }
    if (isHighlighted) {
        col = {0, 100, 230, 255};
        if (isSelected) {
            col = GREEN;
        }
        sampy -= 2;
        sampHeight += 4;
    }

    if (sampx + sampWidth >= SIDEBARTOTALWIDTH && sampx <= GetScreenWidth() + SIDEBARTOTALWIDTH) {
        DrawRectangle(sampx, sampy, sampWidth, sampHeight, col);
        return {sampx, sampy, sampWidth, sampHeight};
    }
    else return {};
}