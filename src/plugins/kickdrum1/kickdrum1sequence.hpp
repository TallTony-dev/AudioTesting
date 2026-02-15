#pragma once
#include "../sequence.hpp"
#include "kickdrum1.hpp"
#include "../plugininterface.hpp"
#include "../include/raylib.h"

class KickDrum1Sequence : public Sequence {
    public:
        float GetSampleAtTime(float time) override;
        void AddSamples(std::shared_ptr<KickDrum> drum, float startTime, float freq, int repetitions = 1, float timeGap = 0);
        void Update(RenderTexture2D tex) override; //called each loop to update any logic in samples
        void Initialize() override;
        void Deinitialize() override;
};

//EXPORT_SEQUENCE(KickDrum1Sequence)