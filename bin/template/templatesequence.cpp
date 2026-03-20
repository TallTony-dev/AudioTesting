#include "templatesequence.hpp"
#include "../rayinclude/raygui.h"

EXPORT_SEQUENCE(TemplateSequence)

void TemplateSequence::LoadSequenceSamples(std::string filePath) {
    Sequence::LoadSequenceSamples(filePath);
}
float TemplateSequence::GetSampleAtTime(float time) {
    return Sequence::GetSampleAtTime(time);
}

void TemplateSequence::Initialize(Vector2 dims) { 
    Sequence::Initialize(dims);
    name = "template";
    hasWindow = true;
    //AddSamples({}, 0, 50, 1);
}

bool showMessageBox;
void TemplateSequence::Update() {
    Sequence::Update();
    //update any custom logic
}
void TemplateSequence::DrawWindowContent() {
    Rectangle currentPos = GetCurrentPos();
    DrawRectangle(0,0,currentPos.width, currentPos.height, RED);
    Sequence::DrawWindowContent();
}

SequenceSample *TemplateSequence::AddSamples(std::unordered_map<std::string, SampleProperty> params, float startTime, int repetitions, float timeGap) {
    Template *samp;
    SequenceSample *seqSamp;
    for (int i = 0; i < repetitions; i++) {
        samp = new Template(params);
        seqSamp = new SequenceSample{samp, startTime + timeGap * i};
        AddSequenceSample(seqSamp, startTime);
    }
    return seqSamp;
}
