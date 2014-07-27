#ifndef CONSOLE_H
#define CONSOLE_H

#include "../gui/widgets/text.h"
#include "../gui/widgets/editbox.h"

#define CONSOLE_LINES	17

class RichText;

void FillConsole();
void ToggleConsole();
void SubmitConsole(RichText* rt);

#endif