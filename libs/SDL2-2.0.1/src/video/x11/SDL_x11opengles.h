/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2013 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
	 claim that you wrote the original software. If you use this software
	 in a product, an acknowledgment in the product documentation would be
	 appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
	 misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_config.h"

#ifndef _SDL_x11opengles_h
#define _SDL_x11opengles_h

#if SDL_VIDEO_OPENGL_EGL

#include "../SDL_sysvideo.h"
#include "../SDL_egl.h"

typedef struct SDL_PrivateGLESData
{
} SDL_PrivateGLESData;

/* OpenGLES functions */
#define X11_GLES_GetAttribute SDL_EGL_GetAttribute
#define X11_GLES_GetProcAddress SDL_EGL_GetProcAddress
#define X11_GLES_UnloadLibrary SDL_EGL_UnloadLibrary
#define X11_GLES_SetSwapInterval SDL_EGL_SetSwapInterval
#define X11_GLES_GetSwapInterval SDL_EGL_GetSwapInterval
#define X11_GLES_DeleteContext SDL_EGL_DeleteContext

extern int X11_GLES_LoadLibrary(_THIS, const char *path);
extern XVisualInfo *X11_GLES_GetVisual(_THIS, Display * display, int screen);
extern SDL_GLContext X11_GLES_CreateContext(_THIS, SDL_Window * window);
extern void X11_GLES_SwapWindow(_THIS, SDL_Window * window);
extern int X11_GLES_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context);

#endif /* SDL_VIDEO_OPENGL_EGL */

#endif /* _SDL_x11opengles_h */

/* vi: set ts=4 sw=4 expandtab: */
