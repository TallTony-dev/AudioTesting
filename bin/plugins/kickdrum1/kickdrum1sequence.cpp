#include "kickdrum1sequence.hpp"
#include "../rayinclude/raygui.h"

EXPORT_SEQUENCE(KickDrum1Sequence)

void KickDrum1Sequence::LoadSequenceSamples(std::string filePath) {
    Sequence::LoadSequenceSamples(filePath);
}
float KickDrum1Sequence::GetSampleAtTime(float time) {
    return Sequence::GetSampleAtTime(time);
}

void KickDrum1Sequence::Initialize(Vector2 dims) { 
    Sequence::Initialize(dims);
    name = "kickdrum1";
    //AddSamples({}, 0, 50, 1);
}

bool showMessageBox;
void KickDrum1Sequence::Update() {
    Sequence::Update();
    //update any custom logic
}
void KickDrum1Sequence::DrawWindowContent() {
    if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#191#Show Message")) 
        showMessageBox = true;
        
    if (showMessageBox)
    {
        int result = GuiMessageBox((Rectangle){ 85, 70, 250, 100 },
            "#191#Message Box", "Hi! This is a message!", "Nice;Cool");

        if (result >= 0) showMessageBox = false;
    }
    Rectangle currentPos = GetCurrentPos();
    DrawRectangle(0,0,currentPos.width, currentPos.height, RED);
}

void KickDrum1Sequence::AddSamples(std::unordered_map<std::string, float> params, float startTime, int repetitions, float timeGap) {
    for (int i = 0; i < repetitions; i++) {
        KickDrum *drum = new KickDrum(params);
        samplesToAdd.push_back(SequenceSample{drum, startTime + timeGap * i});
    }
    //TODO: make sure these are sorted
}
