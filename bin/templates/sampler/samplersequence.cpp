#include "samplersequence.hpp"
#include "../rayinclude/raygui.h"
#include "../include/wav_file_reader.h"

EXPORT_SEQUENCE(SamplerSequence)

void SamplerSequence::LoadSequenceSamples(std::string filePath) {
    Sequence::LoadSequenceSamples(filePath);
}
float SamplerSequence::GetSampleAtTime(double time) {
    return Sequence::GetSampleAtTime(time);
}
void SamplerSequence::LoadSong(std::string songPath) {
    Sequence::LoadSong(songPath);

    
    sakado::WavFileReader wfr(songPath + name + "/" + "sample.wav");
    buf = new float[wfr.NumData];
    bufLength = wfr.NumData;
    wfr.Read(buf, bufLength);
    sampleRate = wfr.SampleRate;
    sampLength = (float)bufLength / (float)sampleRate;
}

inline int SamplerSequence::GetSafeIndexAtTime(float time) {
    int ind = sampleRate * time;
    if (ind < 0) ind = 0;
    if (ind >= bufLength) ind = bufLength - 1;
    return ind;
}
float SamplerSequence::GetSampleAtTime(float time) {
    return buf[GetSafeIndexAtTime(time)];
}

SamplerSequence::~SamplerSequence() {
    Sequence::~Sequence();
    delete buf;
}

void SamplerSequence::Initialize(Vector2 dims) { 
    Sequence::Initialize(dims);
    name = "sampler";
    hasWindow = true;
    //AddSamples({}, 0, 50, 1);
}

bool showMessageBox;
void SamplerSequence::Update() {
    Sequence::Update();
    //update any custom logic
}
void SamplerSequence::DrawWindowContent() {
    Rectangle currentPos = GetCurrentPos();
    DrawRectangle(0,0,currentPos.width, currentPos.height, GREEN);
    Sequence::DrawWindowContent();
}

SequenceSample *SamplerSequence::AddSamples(std::unordered_map<std::string, SampleProperty> params, double startTime, int repetitions, double timeGap) {
    Sampler *samp;
    SequenceSample *seqSamp;
    for (int i = 0; i < repetitions; i++) {
        samp = new Sampler(params, this);
        seqSamp = new SequenceSample{samp, startTime + timeGap * i};
        AddSequenceSample(seqSamp, startTime);
    }
    return seqSamp;
}
