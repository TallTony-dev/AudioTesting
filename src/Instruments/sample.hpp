#pragma once

#define GETFREQ(hz, amplitude, t, phaseDiff) (amplitude * sin(2*M_PI*t*hz + phaseDiff))

class Sample {
    public:
        virtual void GetSample(float time);
        float length;
        float volume;

};