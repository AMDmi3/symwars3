#ifndef EDITORGUI_H
#define EDITORGUI_H

#include "../../common/platform.h"

#define EDTOOL_BORDERS			0
#define EDTOOL_PLACEUNITS		1
#define EDTOOL_PLACEBUILDINGS	2
#define EDTOOL_DELETEOBJECTS	3
#define EDTOOL_PLACEROADS		4
#define EDTOOL_PLACEPOWERLINES	5
#define EDTOOL_PLACECRUDEPIPES	6

void FillEditorGUI();
int GetPlaceUnitType();
int GetPlaceBuildingType();
int GetEdTool();
int GetPlaceUnitCountry();
int GetPlaceUnitCompany();

#endif
