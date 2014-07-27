#ifndef SAVEMAP_H
#define SAVEMAP_H

#define MAP_TAG			{'C','S','M'}
#define MAP_VERSION		3

float ConvertHeight(unsigned char brightness);
void LoadJPGMap(const char* relative);
void FreeMap();
bool LoadMap(const char* fullpath);
bool SaveMap(const char* fullpath);

#endif
