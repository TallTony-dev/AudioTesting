#include "kickdrum1sequence.hpp"
#include "../include/raygui.h"

EXPORT_SEQUENCE(KickDrum1Sequence)

float KickDrum1Sequence::GetSampleAtTime(float time) {
    return Sequence::GetSampleAtTime(time);
}

void KickDrum1Sequence::Initialize(Vector2 dims) { 
    texturePos = {.width = dims.y, .height = dims.x, .x = 100, .y = 100};
    Sequence::Initialize(dims);
}
void KickDrum1Sequence::Deinitialize() { 
    Sequence::Deinitialize();
}

bool showMessageBox;
void KickDrum1Sequence::Update() {
    Sequence::Update();

    if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#191#Show Message")) 
        showMessageBox = true;
        
    if (showMessageBox)
    {
        int result = GuiMessageBox((Rectangle){ 85, 70, 250, 100 },
            "#191#Message Box", "Hi! This is a message!", "Nice;Cool");

        if (result >= 0) showMessageBox = false;
    }

    DrawRectangle(0,0,texturePos.width, texturePos.height, RED);
}

void KickDrum1Sequence::AddSamples(std::vector<float> params, float startTime, float freq, int repetitions, float timeGap) {
    for (int i = 0; i < repetitions; i++) {
        KickDrum *drum = new KickDrum();
        activeSamples.push_back(SequenceSample{drum, startTime + timeGap * i, freq});
    }
}
