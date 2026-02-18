#include "plugins/include/helpers.hpp"
#include <assert.h>
//duty between 0 and 1 and represents portion of time up
float GetSquareWave(float hz, float duty, float amplitude, float phaseDiff, float time) {
    float val = -amplitude;
    float period = 1.0f / hz;
    float adjTime = std::fmod(time + phaseDiff / hz, period);

    if (adjTime < period * duty)
        val += 2 * amplitude;
    return val;
}

float GetBeatTime(float bpm, int beat, int numerator, int denom) {

    float timeBetweenBeats = GetTimeBetweenBeats(bpm, numerator, denom);
    return timeBetweenBeats * beat;
}

float GetTimeBetweenBeats(float bpm, int numerator, int denom) {
    float beatTime = 60 / bpm * (numerator / denom);
    return beatTime;
}