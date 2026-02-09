#include "kickdrum1.hpp"
#include "../../helpers.hpp"

KickDrum::KickDrum() {
    length = 0.5;
    volumeMult = 1;
}

float KickDrum::GetSample(float time, float freqMult) {
    float amp = GETSIN(freqMult * 100 / (time * 5 + 1), 1 / (10 * time * time + 1), time, 0);

    if (length - time < 0.05) {
        amp *= 1.0f - (time - length + 0.05) / 0.05;
    }

    return amp;
}