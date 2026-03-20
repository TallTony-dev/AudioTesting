#include "ambientsequence.hpp"
#include "../rayinclude/raygui.h"

EXPORT_SEQUENCE(Ambient1Sequence)

void Ambient1Sequence::LoadSequenceSamples(std::string filePath) {
    Sequence::LoadSequenceSamples(filePath);
}
float Ambient1Sequence::GetSampleAtTime(float time) {
    return Sequence::GetSampleAtTime(time);
}

void Ambient1Sequence::Initialize(Vector2 dims) { 
    Sequence::Initialize(dims);
    name = "ambient1";
    hasWindow = false;
    //AddSamples({}, 0, 50, 1);
}

bool showMessageBox;
void Ambient1Sequence::Update() {
    Sequence::Update();
    //update any custom logic
}
void Ambient1Sequence::DrawWindowContent() {
    Rectangle currentPos = GetCurrentPos();
    DrawRectangle(0,0,currentPos.width, currentPos.height, RED);
    Sequence::DrawWindowContent();
}

SequenceSample *Ambient1Sequence::AddSamples(std::unordered_map<std::string, SampleProperty> params, float startTime, int repetitions, float timeGap) {
    Ambient1 *samp;
    SequenceSample *seqSamp;
    for (int i = 0; i < repetitions; i++) {
        samp = new Ambient1(params);
        seqSamp = new SequenceSample{samp, startTime + timeGap * i};
        AddSequenceSample(seqSamp, startTime);
    }
    return seqSamp;
}
