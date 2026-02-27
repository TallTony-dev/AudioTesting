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

typedef struct measure {
    float bpm;
    float length;
    int numerator;
    int denominator;
} Measure;

typedef struct sequenceSample {
    Sample *sample;
    float startTime;
} SequenceSample;

enum Edge {None, Bottom, BottomRight, BottomLeft, Left, TopLeft, Top, TopRight, Right, Header };

class Sequence {
    public:
        Sequence(); //empty
        virtual ~Sequence(); //destructor
        //void Sequence::AddSamplesOfLength(std::shared_ptr<Sample> sample, float startTime, float freqMult, float length, int repetitions = 1, float timeGap = 0);
        virtual float GetSampleAtTime(float time); //updates sample logic based off time and gets sample, should be in range 0,1
        virtual void AddSamples(std::unordered_map<std::string, float>, float startTime, int repetitions = 1, float timeGap = 0);
        void DrawWindow(); //draws window
        void DrawSequence(); //draws sequence to back via maingui
        virtual void Update();
        virtual void Initialize(Vector2 dims);
        void LoadSong(std::string songPath);
        std::string name;
        bool isWindowShown; //true if close button hasnt been pressed
        Rectangle GetCurrentPos(); //exclude header
        Rectangle GetCurrentWindowPos(); //include header
        Rectangle GetCurrentWindowPaddedPos(); //include side padding for resize
        std::vector<SequenceSample*> GetAllSamples();
        std::vector<std::tuple<Measure, float>> GetMeasures(); //float is measure start time
    protected:
        virtual void LoadSequenceSamples(std::string filePath); //load sequence file
        virtual void DrawWindowContent(); //called after setting up drawing to window
        //Draw to the window as you normally would draw to any window, as TextureMode is enabled

        std::vector<SequenceSample*> activeSamples;
        std::vector<SequenceSample*> samplesToAdd;
        std::vector<SequenceSample*> samplesAdded;
        void UpdateCurrentPos(Rectangle rect); //updates rendertexture too
        bool isWindowResized;
        Edge selectedEdge = Edge::None;
        float GetBeatTime(int measure, float beat); //measures are 1 indexed
        void SortSamplesToAdd();
    private:
        void AddMeasureToCount(Measure measure);
        void UpdateMeasureTimes();
        //Measure, and time at the start of that measure
        std::vector<std::tuple<Measure, float>> measures; //don't directly add to this
        Rectangle currentPos;
        RenderTexture2D windowTex; 
        void UpdateCurrentPos();
        bool wasIntersecting = false;
};