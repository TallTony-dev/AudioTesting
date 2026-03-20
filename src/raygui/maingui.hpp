#pragma once
#include <string>
#include "raygui.h"
#include "../pluginloader.hpp"

class Sequence; // forward declaration to avoid circular include

#define MEASUREBARWIDTH 5
#define BEATBARWIDTH 2

#define INTERACT_DEFAULT 0
#define INTERACT_ADDNOTE 1
extern int interactionState;

extern bool snapToBeats;
extern bool isClickUsed;

#define TIMESCROLLSENSITIVITY 0.01
#define TIMESCALESENSITIVITY 0.01
extern float timeLeftSide; //time that the left side of the screen represents
extern float timeRightSide; //time that the right side of the screen minus sidebar width represents
float ConvertTimeToXPos(float time);

void UpdateGui(PluginLoader &plugins); //update the gui, some elements only on resize

#define TIMETICKWIDTH 5
#define TIMETICKHEIGHT 12

#define TIMELINEWIDTH 4

#define BOTTOMBARWIDTH 75
#define BOTTOMBARHEIGHT 40
//add to or remove from the bottom bar
void AddToBottomBar(Sequence *seq);
void RemoveFromBottomBar(Sequence *seq); //safely tries to remove if exists
void DrawBottomBar();

#define SIDEBARTOTALWIDTH 40

#define FILEBARHEIGHT 20
#define TOOLBARHEIGHT 20
#define TIMEBARHEIGHT 15
#define TOPBARTOTALHEIGHT (TIMEBARHEIGHT + FILEBARHEIGHT + TOOLBARHEIGHT)

#define SEQUENCEBARBORDERWIDTH 5
#define MINSEQUENCEBARHEIGHT 15
#define SCROLLSENSITIVITY 2.5

void AddToSequenceBar(Sequence *seq);
bool RemoveFromSequenceBar(Sequence *seq); //true if removed
void DrawSequenceBars();

bool IsWindowSelected(Sequence *window);
void SetWindowSelected(Sequence *window);

bool CreateWindowBoxAround(Rectangle rect, std::string name);