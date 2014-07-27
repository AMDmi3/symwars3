#ifndef PLAYGUI_H
#define PLAYGUI_H

class Widget;

void FillPlayGUI();
void BuildMenu_OpenPage1();
void BuildMenu_OpenPage2();
void Click_RightMenu_BackToOpener();
void UpdateResTicker();
void ShowMessage(const RichText& msg);
void Resize_Window(Widget* thisw);

#endif
