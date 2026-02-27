#pragma once
#include <string>
#include "raygui.h"
#include "../pluginloader.hpp"

class Sequence; // forward declaration to avoid circular include

#define BOTTOMBARWIDTH 75
#define BOTTOMBARHEIGHT 40

//add to or remove from the bottom bar
void AddToBottomBar(Sequence *seq);
void RemoveFromBottomBar(Sequence *seq); //safely tries to remove if exists
void DrawBottomBar();


#define SEQUENCEBARHEIGHT 60

void AddToSequenceBar(Sequence *seq);
bool RemoveFromSequenceBar(Sequence *seq); //true if removed
void DrawSequenceBars();

bool IsWindowSelected(Sequence *window);
void SetWindowSelected(Sequence *window);
void UpdateWindowSelection(PluginLoader &plugins);

bool DrawWindowBoxAround(Rectangle rect, std::string name);


