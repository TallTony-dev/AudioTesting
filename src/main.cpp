#include "generatewav.hpp"
#include <math.h>

#define SAMPLERATE  (44100)
#define BUF_SIZE (SAMPLERATE)
 
short int buffer[BUF_SIZE];

#define GETFREQ(hz, amplitude, t, phaseDiff) (amplitude * sin(2*M_PI*t*hz + phaseDiff))

int main(int argc, char ** argv)
{
    float t;
    float amplitude = 32000;
 
    for (int i = 0; i < BUF_SIZE; i++)
    {
        float resultantSample = 0;
        t = (float)i / SAMPLERATE;




        resultantSample += GETFREQ(75, amplitude * cos(t * 4) * ceil(cos(t*4)), t, 0);
        resultantSample += GETFREQ(600 - ceil((t * 10)), amplitude, t, 0);

        buffer[i] = resultantSample;
    }
 
    write_wav("test.wav", BUF_SIZE, buffer, SAMPLERATE);
 
    return 0;
}