#include "generatewav.hpp"
#include <math.h>
#include <cstdio>
#include <memory>
#include "miniaudio/miniaudio.h"
#include "sequence.hpp"
#include "helpers.hpp"
#include "instruments/allinstruments.hpp"

#define BUF_SIZE (SAMPLERATE*10)
 


Sequence mainSequence;
void AddSamples() {
    mainSequence.AddSamples(std::make_shared<KickDrum>(), 0, 1, 50, 1);
    std::shared_ptr<GoopSynth> goop = std::make_shared<GoopSynth>();
    GoopSynth* gooper = goop.get();
    gooper->length = 0.5;
    gooper->attackMult = 1;
    gooper->attackLength = 0.02;
    gooper->volumeMult = 0.8;
    //mainSequence.AddSamples(goop, 0, 0, 40, 1);
    //mainSequence.AddSamples(std::make_shared<HiHat1>(), 0, 1, 50, 1);

}

int ind = 0;
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    float* output = (float*)pOutput;

    for (ma_uint32 i = 0; i < frameCount; i++)
    {
        float resultantSample = 0;
        float t = (float)ind / SAMPLERATE;

        //resultantSample += GETFREQ(600, 0.5f, t, 0);
        resultantSample += mainSequence.GetSampleAtTime(t);

        *output++ = resultantSample;

        ind++;
    }
}

//Some LLM use for the base code for miniaudio output
short int buffer[BUF_SIZE];
int main(int argc, char ** argv)
{
    AddSamples();
    ma_device_config deviceConfig;
    ma_device device;
    
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = ma_format_f32;
    deviceConfig.playback.channels = 1;
    deviceConfig.sampleRate        = SAMPLERATE;
    deviceConfig.dataCallback      = data_callback;
    
    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS)
    {
        printf("Failed to initialize audio device.\n");
        return -1;
    }
    
    if (ma_device_start(&device) != MA_SUCCESS)
    {
        printf("Failed to start audio device.\n");
        ma_device_uninit(&device);
        return -1;
    }
    
    printf("Press Enter to stop...\n");
    getchar();
    
    ma_device_uninit(&device);
    printf("Audio stopped.\n");
    
    //write wav here
    float t;
    float amplitude = 32000;
 
    for (int i = 0; i < BUF_SIZE; i++)
    {
        float resultantSample = 0;
        t = (float)i / SAMPLERATE;

        //resultantSample += GETFREQ(600, amplitude, t, 0);
        resultantSample += BASEAMPLITUDE * mainSequence.GetSampleAtTime(t);

        buffer[i] = resultantSample;
    }
 
    write_wav("out.wav", BUF_SIZE, buffer, SAMPLERATE);
 
    return 0;
}