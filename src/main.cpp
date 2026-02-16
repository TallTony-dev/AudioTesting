#include "generatewav.hpp"
#include <math.h>
#include <cstdio>
#include <memory>
#include <vector>
#include "raygui/raylib/src/external/miniaudio.h"
#include "plugins/sequence.hpp"
#include "helpers.hpp"
#include "plugins/kickdrum1/kickdrum1.hpp"
#include "plugins/kickdrum1/kickdrum1sequence.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui/raygui.h"
#include "pluginloader.hpp"


#define BUF_SIZE (SAMPLERATE*10)
 

PluginLoader loader;


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

//Some LLM use for the base code for miniaudio output
short int buffer[BUF_SIZE];
int main(int argc, char ** argv)
{
    InitWindow(800, 500, "WOah cool DAW buddy");
    SetTargetFPS(60);
    
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

        for (LoadedPlugin plugin : loader.plugins) { //should reference pluginloader instead
            //that rendertex gets passed in with a window border drawn around it that allows resizing
            Sequence *sequence = plugin.sequence;
            Rectangle texturePos = sequence->texturePos;
            Rectangle guiBox = {.width = texturePos.width, .height = texturePos.height + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, 
                                .x = texturePos.x, .y = texturePos.y - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT};
            if (sequence->isWindowShown) {
                if(GuiWindowBox(guiBox, "pluginwindow"))
                    sequence->isWindowShown = false;
                BeginTextureMode(sequence->tex);
                sequence->Update();
                EndTextureMode();
                DrawTexture(sequence->tex.texture, texturePos.x, texturePos.y, WHITE);
            }
        }

        EndDrawing();
    }
    
    ma_device_uninit(&device);
    printf("Audio stopped.\n");
    
    CreateWavFile();
    return 0;
}