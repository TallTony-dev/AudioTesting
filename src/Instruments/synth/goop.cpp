#include "goop.hpp"
#include "../../helpers.hpp"

GoopSynth::GoopSynth() {
    length = 1.0f;
    volumeMult = 1.0f;
    attackMult = 1.0f;
    attackLength = 0.1f;
}

float GoopSynth::GetSample(float time, float freq) {
    if (time < attackLength)
        freq += 1000 * attackMult * (attackLength - time) * (attackLength - time);

    float amp = GETSIN(freq, 1, time, 0);
    return amp;
}