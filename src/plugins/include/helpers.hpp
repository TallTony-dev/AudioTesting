#pragma once
#include <cmath>

#define SAMPLERATE (44100)

#define GETSIN(hz, amplitude, t, phaseDiff) (amplitude * sin(2*M_PI*t*hz + phaseDiff))
#define RANDOMFLOAT ((float)(std::rand()) / (float)(RAND_MAX))
#define BASEAMPLITUDE 32000

//duty between 0 and 1 and represents portion of time up
float GetSquareWave(float hz, float duty, float amplitude, float phaseDiff, float time);

float GetBeatTime(float bpm, int beat, int numerator, int denom);
float GetTimeBetweenBeats(float bpm, int numerator, int denom);