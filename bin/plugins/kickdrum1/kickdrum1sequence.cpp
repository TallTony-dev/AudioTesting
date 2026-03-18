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
    Rectangle currentPos = GetCurrentPos();
    DrawRectangle(0,0,currentPos.width, currentPos.height, RED);
    Sequence::DrawWindowContent();
}

SequenceSample *KickDrum1Sequence::AddSamples(std::unordered_map<std::string, SampleProperty> params, float startTime, int repetitions, float timeGap) {
    KickDrum *samp;
    SequenceSample *seqSamp;
    for (int i = 0; i < repetitions; i++) {
        samp = new KickDrum(params);
        seqSamp = new SequenceSample{samp, startTime + timeGap * i};
        AddSequenceSample(seqSamp, startTime);
    }
    return seqSamp;
}
