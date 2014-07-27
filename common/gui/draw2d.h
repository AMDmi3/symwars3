#ifndef DRAW2D_H
#define DRAW2D_H

void DrawImage(unsigned int tex, float left, float top, float right, float bottom, float texleft=0, float textop=0, float texright=1, float texbottom=1);
void DrawSquare(float r, float g, float b, float a, float left, float top, float right, float bottom);
void DrawLine(float r, float g, float b, float a, float x1, float y1, float x2, float y2);

#endif
