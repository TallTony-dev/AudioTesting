#pragma once
#include "../../sequence.hpp"
#include "kickdrum1.hpp"

class KickDrum1Sequence : public Sequence {
    float GetSampleAtTime(float time);
    void AddSamples(std::shared_ptr<KickDrum> drum, float startTime, float freq, int repetitions = 1, float timeGap = 0);
    void Update(); //called each loop to update any logic in samples
    void Initialize();
    void Deinitialize();
};