#include "playback.hpp"

PluginLoader loader;


bool playing = false;
float currentTime = 0;
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    if (playing) {
    float* output = (float*)pOutput;

        for (ma_uint32 i = 0; i < frameCount; i++)
        {
            float resultantSample = 0;
            currentTime += (float)1 / SAMPLERATE;

            for (LoadedPlugin plugin : loader.plugins) {
                resultantSample += plugin.sequence->GetSampleAtTime(currentTime) * plugin.sequence->seqVolume;
            }

            *output++ = resultantSample;

        }
    }
}

ma_device device;
int InitializePlayback() {
    ma_device_config deviceConfig;
    
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
    return 0;
}

void DeinitializePlayback() {
    ma_device_uninit(&device);
    printf("Audio stopped.\n");
}