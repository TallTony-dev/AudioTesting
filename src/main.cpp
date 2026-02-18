#include "generatewav.hpp"
#include <math.h>
#include <cstdio>
#include <memory>
#include <vector>
#include "raygui/raylib/src/external/miniaudio.h"
#include "plugins/include/sequence.hpp"
#include "plugins/include/helpers.hpp"
#include "raygui/raygui.h"
#include "pluginloader.hpp"
#include "raygui/maingui.hpp"
#include <iostream>


#define BUF_SIZE (SAMPLERATE*10)

PluginLoader loader;

//Some LLM use for the base code for miniaudio output
int ind = 0;
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    float* output = (float*)pOutput;

    for (ma_uint32 i = 0; i < frameCount; i++)
    {
        float resultantSample = 0;
        float t = (float)ind / SAMPLERATE;

        for (LoadedPlugin plugin : loader.plugins) {
            resultantSample += plugin.sequence->GetSampleAtTime(t);
        }

        *output++ = resultantSample;

        ind++;
    }
}

short int buffer[BUF_SIZE];
void CreateWavFile() {
    for (int i = 0; i < BUF_SIZE; i++)
    {
        float resultantSample = 0;
        float t = (float)i / SAMPLERATE;

        for (LoadedPlugin plugin : loader.plugins) {
            resultantSample += BASEAMPLITUDE * plugin.sequence->GetSampleAtTime(t);
        }

        buffer[i] = resultantSample;
    }
    write_wav("out.wav", BUF_SIZE, buffer, SAMPLERATE);
}


int main(int argc, char ** argv)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    InitWindow(800, 500, "Woah cool DAW buddy");

    loader.LoadPlugin("kickdrum1");
    

    
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
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLUE);

        int bottombarcount = 0;
        for (LoadedPlugin plugin : loader.plugins) { //should reference pluginloader instead
            //that rendertex gets passed in with a window border drawn around it that allows resizing
            plugin.sequence->Draw();
        }
        DrawBottomBar();
        EndDrawing();
    }
    
    ma_device_uninit(&device);
    printf("Audio stopped.\n");
    
    CreateWavFile();
    return 0;
}