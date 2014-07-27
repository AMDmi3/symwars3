#ifndef SHADER_H
#define SHADER_H

#include "../platform.h"

#define SSLOT_SHADOWMAP			0
#define SSLOT_LIGHTMATRIX		1
#define SSLOT_LIGHTPOS			2
//#define SSLOT_LIGHTDIR			3
#define SSLOT_TEXTURE0			4
#define SSLOT_POSITION			5
#define SSLOT_NORMAL			6
#define SSLOT_PROJECTION		7
#define SSLOT_MODELMAT			8
#define SSLOT_VIEWMAT			9
#define SSLOT_MODELVIEW         10
#define SSLOT_NORMALMAT			11
#define SSLOT_INVMODLVIEWMAT	12
#define SSLOT_MVP   			13
#define SSLOT_COLOR				14
#define SSLOT_OWNCOLOR			15
#define SSLOT_WIDTH				16
#define SSLOT_HEIGHT			17
#define SSLOT_CAMERAPOS			18
#define SSLOT_SCALE				19
#define SSLOT_MIND				20
#define SSLOT_MAXD				21
#define SSLOT_NORMALMAP			22
#define SSLOT_SPECULARMAP		23
//#define SSLOT_TANGENT			24
#define SSLOT_MAXELEV			25
#define SSLOT_SANDONLYMAXY		26
#define SSLOT_SANDGRASSMAXY		27
#define SSLOT_GRASSONLYMAXY		28
#define SSLOT_GRASSROCKMAXY		29
#define SSLOT_SANDTEX			30
#define SSLOT_GRASSTEX			31
#define SSLOT_SNOWTEX			32
#define SSLOT_ROCKTEX			33
#define SSLOT_ROCKNORMTEX		34
#define SSLOT_CRACKEDROCKTEX	35
#define SSLOT_CRACKEDROCKNORMTEX	36
#define SSLOT_SUNDIRECTION		37
#define SSLOT_GRADIENTTEX		38
#define SSLOT_DETAILTEX			39
#define SSLOT_OWNERMAP			40
#define SSLOT_MAPMINZ			41
#define SSLOT_MAPMAXZ			42
#define SSLOT_VERTCOLORS		43
#define SSLOT_MODELMATS			44
#define SSLOT_ONSWITCHES		45
#define SSLOT_MAPMINX			46
#define SSLOT_MAPMAXX			47
#define SSLOT_MAPMINY			48
#define SSLOT_MAPMAXY			49
#define SSLOT_WAVEPHASE			50
#define SSLOT_TEXCOORD0			51
#define SSLOTS					52

class Shader
{
public:
	Shader()	{			 }
	~Shader()
	{
		release();
	}

	GLint getuniform(const char* strVariable);
	GLint getattrib(const char* strVariable);

	void mapuniform(int slot, const char* variable);
	void mapattrib(int slot, const char* variable);

	void release();

	bool m_hastexcoords;
	bool m_hasnormals;
	GLint m_slot[SSLOTS];

	GLhandleARB m_vertshader;
	GLhandleARB m_fragshader;
	GLhandleARB m_program;
};

#define SHADER_DEPTH			0
#define SHADER_OWNED			1
#define SHADER_ORTHO			2
#define SHADER_COLOR2D			3
#define SHADER_COLOR3D			4
#define SHADER_BILLBOARD		5
#define SHADER_MAPTILES			6
#define SHADER_WATER			7
#define SHADER_DEPTHTRANSP		8
#define SHADER_FOLIAGE			9
#define SHADER_MAPTILESMM		10
#define SHADER_WATERMM			11
#define SHADER_RIM				13
#define SHADER_SKYBOX			14
#define SHADER_UNIT				15
#define SHADERS					16

extern Shader g_shader[SHADERS];
extern int g_curS;

void UseS(int shader);
void EndS();
void InitGLSL();
void TurnOffShader();
void ReleaseShaders();
std::string LoadTextFile(char* strFile);
void LoadShader(int shader, char* strVertex, char* strFragment, bool hastexcoords, bool hasnormals);

#endif


