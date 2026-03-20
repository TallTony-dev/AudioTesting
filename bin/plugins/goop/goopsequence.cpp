#include "goopsequence.hpp"
#include "../rayinclude/raygui.h"

EXPORT_SEQUENCE(GoopSequence)

void GoopSequence::LoadSequenceSamples(std::string filePath) {
    Sequence::LoadSequenceSamples(filePath);
}
float GoopSequence::GetSampleAtTime(double time) {
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
    
    Rectangle currentPos = GetCurrentPos();
    DrawRectangle(0,0,currentPos.width, currentPos.height, BLUE);
    Sequence::DrawWindowContent();
}

SequenceSample *GoopSequence::AddSamples(std::unordered_map<std::string, SampleProperty> params, double startTime, int repetitions, double timeGap) {
    Goop *samp;
    SequenceSample *seqSamp;
    for (int i = 0; i < repetitions; i++) {
        samp = new Goop(params);
        seqSamp = new SequenceSample{samp, startTime + timeGap * i};
        AddSequenceSample(seqSamp, startTime);
    }
    return seqSamp;
}
