#include "samplersequence.hpp"
#include "../rayinclude/raygui.h"
#include "../include/wav_file_reader.h"
#include "sampler.hpp"
#include <filesystem>
#include <iostream>

EXPORT_SEQUENCE(SamplerSequence)

void SamplerSequence::LoadSequenceSamples(std::string filePath) {
    Sequence::LoadSequenceSamples(filePath);
}
float SamplerSequence::GetSampleAtTime(double time) {
    return Sequence::GetSampleAtTime(time);
}
void SamplerSequence::LoadSong(std::string songPath) {
    Sequence::LoadSong(songPath);

    delete[] buf;
    buf = nullptr;
    bufLength = 0;
    sampleRate = 0;
    sampLength = 0;

    namespace fs = std::filesystem;
    fs::path relativePath = fs::path(songPath) / name / "sample.wav";
    fs::path resolvedPath = relativePath;

    if (!fs::exists(resolvedPath)) {
        fs::path candidateFromCwd = fs::current_path() / relativePath;
        fs::path candidateFromParent = fs::current_path().parent_path() / relativePath;
        if (fs::exists(candidateFromCwd)) {
            resolvedPath = candidateFromCwd;
        } else if (fs::exists(candidateFromParent)) {
            resolvedPath = candidateFromParent;
        }
    }

    if (!fs::exists(resolvedPath)) {
        std::cerr << "[SamplerSequence] sample.wav not found. Tried: "
                  << relativePath.string() << " and cwd/parent variants" << std::endl;
        return;
    }

    std::cout << "[SamplerSequence] Loading sample from " << resolvedPath.string() << std::endl;
    try {
        sakado::WavFileReader wfr(resolvedPath.string());
        bufLength = static_cast<int>(wfr.NumData);
        sampleRate = static_cast<int>(wfr.SampleRate);
        buf = new float[bufLength];
        wfr.Read(buf, bufLength);

        if (wfr.BitsPerSample == 16) {
            for (int i = 0; i < bufLength; i++) {
                buf[i] = buf[i] / 32768.0f;
            }
        } else if (wfr.BitsPerSample == 8) {
            for (int i = 0; i < bufLength; i++) {
                buf[i] = (buf[i] / 127.5f) - 1.0f;
            }
        }

        for (int i = 0; i < bufLength; i++) {
            if (buf[i] < -1.0f) {
                buf[i] = -1.0f;
            } else if (buf[i] > 1.0f) {
                buf[i] = 1.0f;
            }
        }

        sampLength = sampleRate > 0 ? (float)bufLength / (float)sampleRate : 0;
    } catch (const std::exception& e) {
        std::cerr << "[SamplerSequence] Failed to load WAV at " << resolvedPath.string()
                  << " with error: " << e.what() << std::endl;
        delete[] buf;
        buf = nullptr;
        bufLength = 0;
        sampleRate = 0;
        sampLength = 0;
    }
}

inline int SamplerSequence::GetSafeIndexAtTime(float time) {
    if (bufLength <= 0 || sampleRate <= 0) {
        return 0;
    }
    int ind = sampleRate * time;
    if (ind < 0) ind = 0;
    if (ind >= bufLength) ind = bufLength - 1;
    return ind;
}
float SamplerSequence::GetLoadedSampleAtTime(float time) {
    if (buf == nullptr || bufLength <= 0) {
        return 0.0f;
    }
    return buf[GetSafeIndexAtTime(time)];
}

SamplerSequence::~SamplerSequence() {
    delete[] buf;
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
    DrawRectangle(0,0,currentPos.width, 60, DARKPURPLE);
    int scaleFactor = bufLength / currentPos.width;
    if (scaleFactor > 0) {
        for (int i = 0; i < bufLength; i += 1) {
            DrawRectangle(i / scaleFactor, buf[i] * 30 + 30, 1, 2, YELLOW);
        }
    }


    Sequence::DrawWindowContent();
}

std::unordered_map<std::string, SampleProperty> SamplerSequence::GetDefaultProperties() {
    return std::unordered_map<std::string, SampleProperty>{{"vol", {1,0,1}}, 
    {"start", {0,0,sampLength}},{"end", {sampLength,0,sampLength}}, {"len", {sampLength,0,sampLength}}};
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
