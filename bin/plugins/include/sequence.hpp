#pragma once
#include <vector>
#include <memory>
#include <fstream>
#include <queue>
#include "sample.hpp"
#include "../rayinclude/raylib.h"

#define SEQWINDOWINTERACTWIDTH 6
#define SEQWINDOWMINHEIGHT 48
#define SEQWINDOWMINWIDTH 32

typedef struct sequenceSample {
    Sample *sample;
    float startTime;
    float freqMult;
} SequenceSample;

enum Edge {None, Bottom, BottomRight, BottomLeft, Left, TopLeft, Top, TopRight, Right, Header };

class Sequence {
    public:
        Sequence(std::string relFilePath);
        Sequence(); //empty
        virtual ~Sequence(); //destructor
        //void Sequence::AddSamplesOfLength(std::shared_ptr<Sample> sample, float startTime, float freqMult, float length, int repetitions = 1, float timeGap = 0);
        virtual float GetSampleAtTime(float time); //updates sample logic based off time and gets sample, should be in range 0,1
        virtual void AddSamples(std::vector<float> params, float startTime, float freq, int repetitions = 1, float timeGap = 0);
        void Draw(); //starts draw to texture
        virtual void Update();
        virtual void Initialize(Vector2 dims);
        std::string name;
        bool isWindowShown; //true if close button hasnt been pressed
    protected:
        virtual void DrawWindowContent(); //called after setting up drawing to window
        //Draw to the window as you normally would draw to any window, as TextureMode is enabled
        int currentMeasure = 1;
        float measureTimeTotal;
        void AddMeasureToCount(float currentTime);
        Sample *sampleType;
        std::vector<SequenceSample> activeSamples;
        std::vector<SequenceSample> samplesToAdd;
        std::vector<SequenceSample> samplesAdded;
        void UpdateCurrentPos(Rectangle rect); //updates rendertexture too
        Rectangle GetCurrentPos();
        bool isWindowResized;
        Edge selectedEdge = Edge::None;
    private:
        Rectangle currentPos;
        RenderTexture2D windowTex; 
        void UpdateWindowPos();
        bool wasIntersecting = false;
};