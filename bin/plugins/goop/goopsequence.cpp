#include "goopsequence.hpp"
#include "../rayinclude/raygui.h"

EXPORT_SEQUENCE(GoopSequence)

void GoopSequence::LoadSequenceSamples(std::string filePath) {
    Sequence::LoadSequenceSamples(filePath);
}
float GoopSequence::GetSampleAtTime(float time) {
    return Sequence::GetSampleAtTime(time);
}

void GoopSequence::Initialize(Vector2 dims) { 
    Sequence::Initialize(dims);
    name = "goop";
    //AddSamples({}, 0, 50, 1);
}

bool showMessageBox;
void GoopSequence::Update() {
    Sequence::Update();
    //update any custom logic
}
void GoopSequence::DrawWindowContent() {
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

void GoopSequence::AddSamples(std::unordered_map<std::string, float> params, float startTime, int repetitions, float timeGap) {
    for (int i = 0; i < repetitions; i++) {
        Goop *goop = new Goop(params);
        samplesToAdd.push_back(SequenceSample{goop, startTime + timeGap * i});
    }
    //TODO: make sure these are sorted
}
