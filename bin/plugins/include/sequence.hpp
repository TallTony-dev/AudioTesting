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

typedef struct drawnSample {
    SequenceSample *samp;
    Rectangle rect;
} DrawnSample;

extern bool isClickUsed; //redefinition of the thing in maingui.hpp for use by other seqs
extern float currentTime; //redefinition of the thing in manigui.hpp for use by other seqs

enum Edge {None, Bottom, BottomRight, BottomLeft, Left, TopLeft, Top, TopRight, Right, Header };


class Sequence {
    public:
        Sequence(); //empty
        virtual ~Sequence(); //destructor
        //void Sequence::AddSamplesOfLength(std::shared_ptr<Sample> sample, float startTime, float freqMult, float length, int repetitions = 1, float timeGap = 0);
        virtual float GetSampleAtTime(float time); //updates sample logic based off time and gets sample, should be in range 0,1
        virtual SequenceSample * AddSamples(std::unordered_map<std::string, SampleProperty>, float startTime, int repetitions = 1, float timeGap = 0);
        void DrawWindow(); //draws window
        //void DrawSequence(); //draws sequence to back via maingui
        virtual void Update();
        virtual void Initialize(Vector2 dims);
        void LoadSong(std::string songPath);
        void SaveSong(std::string songPath);
        void SaveLoadedSong();
        std::string name;
        bool isWindowShown; //true if close button hasnt been pressed
        bool hasWindow = true; // can be set to false in derived classes to not have window box
        Rectangle GetCurrentPos(); //exclude header
        Rectangle GetCurrentWindowPos(); //include header
        Rectangle GetCurrentWindowPaddedPos(); //include side padding for resize
        std::vector<SequenceSample*> GetAllSamples(); // orders samples 
        std::vector<std::tuple<Measure, float>> GetMeasures(); //float is measure start time
        std::vector<DrawnSample> lastDrawnSamples;
        SequenceSample *selectedSamp = nullptr;
        SequenceSample *ghostSamp = nullptr;
        std::tuple<Measure, float> *GetMeasureAtTime(float time);
        float seqHeight;
        float seqYPos;
        bool wasSeqResizeSelected;
        bool wasSelectedSampMoveSelected;
    protected:
        virtual void LoadSequenceSamples(std::string filePath); //load sequence file
        virtual void DrawWindowContent(); //called after setting up drawing to window
        //Draw to the window as you normally would draw to any window, as TextureMode is enabled

        std::vector<SequenceSample*> activeSamples;
        std::vector<SequenceSample*> samplesToAdd; // low to high (front to back)
        std::vector<SequenceSample*> samplesAdded; // high to low (front to back)
        void AddSequenceSample(SequenceSample *, float startTime); //ONLY CALL WITH PREPOPULATED SAMPLE
        void UpdateCurrentPos(Rectangle rect); //updates rendertexture too
        bool isWindowResized;
        Edge selectedEdge = Edge::None;
        float GetBeatTime(int measure, float beat); //measures are 1 indexed
        float GetBeatAtTime(float time); //returns beat in the measure at time
        void SortSamplesToAdd();
        void SortSamplesAdded();
        float prevTime = 0;
    private:
        std::string loadedSongPath;
        void AddMeasureToCount(Measure measure);
        void UpdateMeasureTimes();
        //Measure, and time at the start of that measure
        std::vector<std::tuple<Measure, float>> measures; //don't directly add to this
        Rectangle currentPos;
        RenderTexture2D windowTex; 
        void UpdateCurrentPos();
        bool wasWindowIntersecting = false;
};