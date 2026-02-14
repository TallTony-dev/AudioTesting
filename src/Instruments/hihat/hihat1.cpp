#include "hihat1.hpp"
#include "../../helpers.hpp"
#include <random>

HiHat1::HiHat1() {
    length = 1;
    volumeMult = 1;
}

float HiHat1::GetSample(float time, float freq) {
    //float amp = GETSIN(100, (0.9 + RANDOMFLOAT * 0.3) / (10 * time + 1), time, 0);
        float amp = GetSquareWave(100, 0.1, 1, 0, time);

    //float amp = GetSquareWave(100, 0.5 + RANDOMFLOAT /(40 * time + 1), 1 / (time * 10 + 1), 0, time);
    //amp += GetSquareWave(120, 0.5 + RANDOMFLOAT /(40 * time + 1), 1 / (time * 10 + 1), 0, time);
    //amp += GetSquareWave(150, 0.5 + RANDOMFLOAT /(40 * time + 1), 1 / (time * 10 + 1), 0, time);

    //amp += GetSquareWave(288, 0.5, 1, 0.2, time);
    //amp = lastSample * 0.8 + amp * 0.2;

    if (length - time < 0.01) {
        amp *= 1.0f - (time - length + 0.05) / 0.05;
    }
    lastSample = amp;
    return amp;
}