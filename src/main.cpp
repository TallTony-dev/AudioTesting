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
#include "playback.hpp"
#include <iostream>


#define BUF_SIZE (SAMPLERATE*10)


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
    loader.LoadPlugin("goop");
    loader.LoadPlugin("ambient1");
    loader.LoadPlugin("sampler");

    if (loader.plugins.size() > 0) {
        for (LoadedPlugin plugin : loader.plugins) {
            plugin.sequence->LoadSong("./songs/song1/");
        }
    }
    else {
        std::cout << "Plugins not loaded?";
    }

    if (InitializePlayback() != 0) {
        std::cout << "Initializing playback failed";
        return -1;
    }

    
    while (!WindowShouldClose()) {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        isClickUsed = false;
        for (LoadedPlugin plugin : loader.plugins) {
            plugin.sequence->Update();
        }
        UpdateGui(loader);
        BeginDrawing();
        ClearBackground(BLUE);
        
        for (LoadedPlugin plugin : loader.plugins) {
            plugin.sequence->DrawWindow();
        }

        
        DrawBottomBar();
        EndDrawing();
    }
    DeinitializePlayback();
    CreateWavFile();
    return 0;
}