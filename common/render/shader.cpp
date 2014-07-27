#include "../utils.h"
#include "shader.h"
#include "../platform.h"
#include "../window.h"
#include "../sim/player.h"
#include "../debug.h"

Shader g_shader[SHADERS];
int g_curS = 0;

GLint Shader::getuniform(const char* strVariable)
{
	if(!m_program)
		return -1;

	return glGetUniformLocation(m_program, strVariable);
}

GLint Shader::getattrib(const char* strVariable)
{
	g_log<<"shader "<<(int)(this-g_shader)<<" attrib "<<strVariable<<" = ";

	if(!m_program)
		return -1;

	g_log<<glGetAttribLocation(m_program, strVariable)<<std::endl;

	return glGetAttribLocation(m_program, strVariable);
}

void Shader::mapuniform(int slot, const char* variable)
{
	m_slot[slot] = getuniform(variable);
	//g_log<<"\tmap uniform "<<variable<<" = "<<(int)m_slot[slot]<<std::endl;
}

void Shader::mapattrib(int slot, const char* variable)
{
	m_slot[slot] = getattrib(variable);
	//g_log<<"\tmap attrib "<<variable<<" = "<<(int)m_slot[slot]<<std::endl;
}

void GetGLVersion(int* major, int* minor)
{
	// for all versions
	char* ver = (char*)glGetString(GL_VERSION); // ver = "3.2.0"

	char vermaj[4];

	for(int i=0; i<4; i++)
	{
		if(ver[i] != '.')
			vermaj[i] = ver[i];
		else
			vermaj[i] = '\0';
	}

	//*major = ver[0] - '0';
	*major = StrToInt(vermaj);
	if( *major >= 3)
	{
		// for GL 3.x
		glGetIntegerv(GL_MAJOR_VERSION, major); // major = 3
		glGetIntegerv(GL_MINOR_VERSION, minor); // minor = 2
	}
	else
	{
		*minor = ver[2] - '0';
	}

	// GLSL
	ver = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION); // ver = "1.50 NVIDIA via Cg compiler"
}

void InitGLSL()
{
	//strstr("abab", "ba");
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if( glewError != GLEW_OK )
	{
		ErrorMessage("Error initializing GLEW!", (const char*)glewGetErrorString( glewError ));
		return;
	}

	g_log<<"Renderer: "<<(char*)glGetString(GL_RENDERER)<<std::endl;
	g_log<<"GL_VERSION = "<<(char*)glGetString(GL_VERSION)<<std::endl;

	if( !GLEW_VERSION_3_0 )
	{
		ErrorMessage("Error", "OpenGL 3.0 not supported!\n" );
		g_quit = true;
		return;
	}

#if 1
	char* szGLExtensions = (char*)glGetString(GL_EXTENSIONS);

	g_log<<szGLExtensions<<std::endl;
	g_log.flush();

	if(!strstr(szGLExtensions, "GL_ARB_debug_output"))
	{
		//ErrorMessage("Error", "GL_ARB_debug_output extension not supported!");
		//g_quit = true;
		//return;
		g_log<<"GL_ARB_debug_output extension not supported"<<std::endl;
	}
	else
	{
		g_log<<"Reging debug handler"<<std::endl;
		g_log.flush();
		glDebugMessageCallbackARB(&GLMessageHandler, 0);
		CheckGLError(__FILE__, __LINE__);
	}

	if(!strstr(szGLExtensions, "GL_ARB_shader_objects"))
	{
		ErrorMessage("Error", "GL_ARB_shader_objects extension not supported!");
		g_quit = true;
		return;
	}

	if(!strstr(szGLExtensions, "GL_ARB_shading_language_100"))
	{
		ErrorMessage("Error", "GL_ARB_shading_language_100 extension not supported!");
		g_quit = true;
		return;
	}
#endif

	int major, minor;
	GetGLVersion(&major, &minor);

	if(major < 3 || ( major == 3 && minor < 0 ))
	{
		ErrorMessage("Error", "OpenGL 3.0 is not supported!");
		g_quit = true;
	}

	LoadShader(SHADER_ORTHO, "shaders/ortho.vert", "shaders/ortho.frag", true, false);
	LoadShader(SHADER_COLOR2D, "shaders/color2d.vert", "shaders/color2d.frag", false, false);
	LoadShader(SHADER_COLOR3D, "shaders/color3d.vert", "shaders/color3d.frag", true, false);
	LoadShader(SHADER_BILLBOARD, "shaders/billboard.vert", "shaders/billboard.frag", true, false);
	LoadShader(SHADER_DEPTH, "shaders/depth.vert", "shaders/depth.frag", true, false);
	LoadShader(SHADER_DEPTHTRANSP, "shaders/depth.vert", "shaders/depthtransp.frag", true, false);
	LoadShader(SHADER_OWNED, "shaders/owned.vert", "shaders/owned.frag", true, true);
	//LoadShader(SHADER_MODEL, "shaders/building.vert", "shaders/building.frag");
	//LoadShader(SHADER_MAPTILES, "shaders/maptilesmegatex.vert", "shaders/maptilesmegatex.frag");
	LoadShader(SHADER_MAPTILES, "shaders/maptiles.vert", "shaders/maptiles.frag", true, true);
	LoadShader(SHADER_WATER, "shaders/water.vert", "shaders/water.frag", true, true);
	LoadShader(SHADER_FOLIAGE, "shaders/foliage.vert", "shaders/foliage.frag", true, true);
	LoadShader(SHADER_MAPTILESMM, "shaders/maptilesmm2.vert", "shaders/maptilesmm2.frag", true, true);
	LoadShader(SHADER_WATERMM, "shaders/watermm.vert", "shaders/watermm.frag", true, true);
	LoadShader(SHADER_RIM, "shaders/rim.vert", "shaders/rim.frag", true, true);
	LoadShader(SHADER_SKYBOX, "shaders/skybox.vert", "shaders/skybox.frag", true, false);
	LoadShader(SHADER_UNIT, "shaders/unit.vert", "shaders/unit.frag", true, true);
}

std::string LoadTextFile(char* strFile)
{
	std::ifstream fin(strFile);

	if(!fin)
	{
		g_log<<"Failed to load file "<<strFile<<std::endl;
		return "";
	}

	std::string strLine = "";
	std::string strText = "";

	while(getline(fin, strLine))
		strText = strText + "\n" + strLine;

	fin.close();

	return strText;
}

void LoadShader(int shader, char* strVertex, char* strFragment, bool hastexcoords, bool hasnormals)
{
	Shader* s = &g_shader[shader];
	std::string strVShader, strFShader;

	if(s->m_vertshader || s->m_fragshader || s->m_program)
		s->release();

    s->m_hastexcoords = hastexcoords;
    s->m_hasnormals = hasnormals;
	s->m_vertshader = glCreateShader(GL_VERTEX_SHADER);
	s->m_fragshader = glCreateShader(GL_FRAGMENT_SHADER);

	strVShader = LoadTextFile(strVertex);
	strFShader = LoadTextFile(strFragment);

	const char* szVShader = strVShader.c_str();
	const char* szFShader = strFShader.c_str();

	glShaderSource(s->m_vertshader, 1, &szVShader, NULL);
	glShaderSource(s->m_fragshader, 1, &szFShader, NULL);

	glCompileShader(s->m_vertshader);
	GLint logLength;
	glGetShaderiv(s->m_vertshader, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		GLchar* log = (GLchar*)malloc(logLength);

		if(!log)
		{
			OutOfMem(__FILE__, __LINE__);
			return;
		}

		glGetShaderInfoLog(s->m_vertshader, logLength, &logLength, log);
		g_log<<"Shader "<<strVertex<<" compile log: "<<std::endl<<log<<std::endl;
		free(log);
	}

	glCompileShader(s->m_fragshader);
	glGetShaderiv(s->m_fragshader, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		GLchar* log = (GLchar*)malloc(logLength);

		if(!log)
		{
			OutOfMem(__FILE__, __LINE__);
			return;
		}

		glGetShaderInfoLog(s->m_fragshader, logLength, &logLength, log);
		g_log<<"Shader "<<strFragment<<" compile log: "<<std::endl<<log<<std::endl;
		free(log);
	}

	s->m_program = glCreateProgram();
	glAttachShader(s->m_program, s->m_vertshader);
	glAttachShader(s->m_program, s->m_fragshader);
	glLinkProgram(s->m_program);

	//glUseProgramObject(s->m_program);

	//g_log<<"shader "<<strVertex<<","<<strFragment<<std::endl;

	g_log<<"Program "<<strVertex<<" / "<<strFragment<<" :";

	glGetProgramiv(s->m_program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar* log = (GLchar*)malloc(logLength);
		glGetProgramInfoLog(s->m_program, logLength, &logLength, log);
		g_log<<"Program link log:"<<std::endl<<log<<std::endl;
		free(log);
	}

	GLint status;
	glGetProgramiv(s->m_program, GL_LINK_STATUS, &status);
	if (status == 0)
	{
		g_log<<"link status 0"<<std::endl;
	}
	else
	{
		g_log<<"link status ok"<<std::endl;
	}

	g_log<<std::endl<<std::endl;


	s->mapattrib(SSLOT_POSITION, "position");
	s->mapattrib(SSLOT_NORMAL, "normalIn");
	s->mapattrib(SSLOT_TEXCOORD0, "texCoordIn0");
	//s->mapattrib(SSLOT_TEXCOORD1, "texCoordIn1");
	s->mapattrib(SSLOT_VERTCOLORS, "vertcolors");
	//s->mapattrib(SSLOT_TANGENT, "tangent");
	s->mapuniform(SSLOT_SHADOWMAP, "shadowmap");
	s->mapuniform(SSLOT_LIGHTMATRIX, "lightMatrix");
	s->mapuniform(SSLOT_LIGHTPOS, "lightPos");
	//s->mapuniform(SSLOT_LIGHTDIR, "lightdir");
	s->mapuniform(SSLOT_TEXTURE0, "texture0");
	s->mapuniform(SSLOT_NORMALMAP, "normalmap");
	s->mapuniform(SSLOT_SPECULARMAP, "specularmap");
	s->mapuniform(SSLOT_PROJECTION, "projection");
	s->mapuniform(SSLOT_MODELMAT, "model");
	s->mapuniform(SSLOT_VIEWMAT, "view");
	s->mapuniform(SSLOT_MVP, "mvp");
	s->mapuniform(SSLOT_MODELVIEW, "modelview");
	s->mapuniform(SSLOT_NORMALMAT, "normalMat");
	//s->mapuniform(SSLOT_INVMODLVIEWMAT, "invModelView");
	s->mapuniform(SSLOT_COLOR, "color");
	s->mapuniform(SSLOT_OWNCOLOR, "owncolor");
	s->mapuniform(SSLOT_WIDTH, "width");
	s->mapuniform(SSLOT_HEIGHT, "height");
	s->mapuniform(SSLOT_MIND, "mind");
	s->mapuniform(SSLOT_MAXD, "maxd");
	s->mapuniform(SSLOT_CAMERAPOS, "cameraPos");
	s->mapuniform(SSLOT_SCALE, "scale");
	s->mapuniform(SSLOT_MAXELEV, "maxelev");
	s->mapuniform(SSLOT_SANDONLYMAXY, "sandonlymaxy");
	s->mapuniform(SSLOT_SANDGRASSMAXY, "sandgrassmaxy");
	s->mapuniform(SSLOT_GRASSONLYMAXY, "grassonlymaxy");
	s->mapuniform(SSLOT_GRASSROCKMAXY, "dirtrockmaxy");
	s->mapuniform(SSLOT_SANDTEX, "sandtex");
	s->mapuniform(SSLOT_GRASSTEX, "grasstex");
	s->mapuniform(SSLOT_SNOWTEX, "snowtex");
	s->mapuniform(SSLOT_ROCKTEX, "rocktex");
	s->mapuniform(SSLOT_ROCKNORMTEX, "rocknormtex");
	s->mapuniform(SSLOT_CRACKEDROCKTEX, "crackedrocktex");
	s->mapuniform(SSLOT_CRACKEDROCKNORMTEX, "crackedrocknormtex");
	s->mapuniform(SSLOT_SUNDIRECTION, "sundirection");
	s->mapuniform(SSLOT_GRADIENTTEX, "gradienttex");
	s->mapuniform(SSLOT_DETAILTEX, "detailtex");
	s->mapuniform(SSLOT_OWNERMAP, "ownermap");
	s->mapuniform(SSLOT_MAPMINZ, "mapminz");
	s->mapuniform(SSLOT_MAPMAXZ, "mapmaxz");
	s->mapuniform(SSLOT_MODELMATS, "modelmats");
	s->mapuniform(SSLOT_ONSWITCHES, "onswitches");
	s->mapuniform(SSLOT_MAPMINX, "mapminx");
	s->mapuniform(SSLOT_MAPMAXX, "mapmaxx");
	s->mapuniform(SSLOT_MAPMINY, "mapminy");
	s->mapuniform(SSLOT_MAPMAXY, "mapmaxy");
	s->mapuniform(SSLOT_WAVEPHASE, "wavephase");
}

void UseS(int shader)
{
	CheckGLError(__FILE__, __LINE__);
	g_curS = shader;

	Shader* s = &g_shader[g_curS];

	//glUseProgramObject(g_shader[shader].m_program);
	glUseProgram(s->m_program);
	CheckGLError(__FILE__, __LINE__);

	Player* py = &g_player[g_curP];
	
	CheckGLError(__FILE__, __LINE__);
	if(s->m_slot[SSLOT_POSITION] != -1)	glEnableVertexAttribArray(s->m_slot[SSLOT_POSITION]);
	CheckGLError(__FILE__, __LINE__);
	if(s->m_slot[SSLOT_TEXCOORD0] != -1) glEnableVertexAttribArray(s->m_slot[SSLOT_TEXCOORD0]);
	CheckGLError(__FILE__, __LINE__);
	if(s->m_slot[SSLOT_NORMAL] != -1)	glEnableVertexAttribArray(s->m_slot[SSLOT_NORMAL]);
	CheckGLError(__FILE__, __LINE__);

	if(s->m_slot[SSLOT_MIND] != -1) glUniform1f(s->m_slot[SSLOT_MIND], MIN_DISTANCE);
	if(s->m_slot[SSLOT_MAXD] != -1) glUniform1f(s->m_slot[SSLOT_MAXD], MAX_DISTANCE / py->zoom);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EndS()
{
	CheckGLError(__FILE__, __LINE__);

	if(g_curS < 0)
		return;

	Shader* s = &g_shader[g_curS];

	CheckGLError(__FILE__, __LINE__);
	if(s->m_slot[SSLOT_POSITION] != -1)	glDisableVertexAttribArray(s->m_slot[SSLOT_POSITION]);
	CheckGLError(__FILE__, __LINE__);
	if(s->m_slot[SSLOT_TEXCOORD0] != -1) glDisableVertexAttribArray(s->m_slot[SSLOT_TEXCOORD0]);
	CheckGLError(__FILE__, __LINE__);
	if(s->m_slot[SSLOT_NORMAL] != -1)	glDisableVertexAttribArray(s->m_slot[SSLOT_NORMAL]);
	CheckGLError(__FILE__, __LINE__);

	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//glDisableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);

	g_curS = -1;
}

void Shader::release()
{
	if(m_vertshader)
	{
		glDetachShader(m_program, m_vertshader);
		glDeleteShader(m_vertshader);
		m_vertshader = NULL;
	}

	if(m_fragshader)
	{
		glDetachShader(m_program, m_fragshader);
		glDeleteShader(m_fragshader);
		m_fragshader = NULL;
	}

	if(m_program)
	{
		glDeleteProgram(m_program);
		m_program = NULL;
	}
}

void TurnOffShader()
{
	glUseProgram(0);
}

void ReleaseShaders()
{
	for(int i=0; i<SHADERS; i++)
		g_shader[i].release();
}

