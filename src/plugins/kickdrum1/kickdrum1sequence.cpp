#include "kickdrum1sequence.hpp"
#include "../include/raygui.h"
#include "../include/raylib.h"

float KickDrum1Sequence::GetSampleAtTime(float time) {
    return Sequence::GetSampleAtTime(time);
}

void KickDrum1Sequence::Initialize() { }
void KickDrum1Sequence::Deinitialize() { }

bool showMessageBox;
void KickDrum1Sequence::Update(RenderTexture2D tex) {
    if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#191#Show Message")) showMessageBox = true;
        if (showMessageBox)
        {
            int result = GuiMessageBox((Rectangle){ 85, 70, 250, 100 },
                "#191#Message Box", "Hi! This is a message!", "Nice;Cool");

            if (result >= 0) showMessageBox = false;
        }
}

void KickDrum1Sequence::AddSamples(std::shared_ptr<KickDrum> drum, float startTime, float freq, int repetitions, float timeGap) {
    Sequence::AddSamples(drum, startTime, freq, repetitions, timeGap);
}
