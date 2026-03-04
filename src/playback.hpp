#include "raygui/raylib/src/external/miniaudio.h"
#include "pluginloader.hpp"
#define SAMPLERATE (44100)

extern bool playing;
extern float currentTime;
extern PluginLoader loader;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
int InitializePlayback();
void DeinitializePlayback();