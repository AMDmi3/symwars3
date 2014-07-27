#include "../platform.h"
#include "../window.h"
#include "../utils.h"
#include "../texture.h"
#include "../sim/player.h"

void SaveScreenshot()
{
	Player* py = &g_player[g_curP];

	LoadedTex screenshot;
	screenshot.channels = 3;
	screenshot.sizeX = py->width;
	screenshot.sizeY = py->height;
	screenshot.data = (unsigned char*)malloc( sizeof(unsigned char) * py->width * py->height * 3 );

	if(!screenshot.data)
	{
		OutOfMem(__FILE__, __LINE__);
		return;
	}

	memset(screenshot.data, 0, py->width * py->height * 3);

	glReadPixels(0, 0, py->width, py->height, GL_RGB, GL_UNSIGNED_BYTE, screenshot.data);

	FlipImage(&screenshot);

	char relative[256];
	std::string datetime = FileDateTime();
	sprintf(relative, "screenshots/%s.jpg", datetime.c_str());
	char fullpath[MAX_PATH+1];
	FullPath(relative, fullpath);

	g_log<<"Writing screenshot "<<fullpath<<std::endl;
	g_log.flush();

	SaveJPEG(fullpath, &screenshot, 0.9f);

	//free(screenshot.data);
}
