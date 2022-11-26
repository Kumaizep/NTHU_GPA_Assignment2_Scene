#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "TinyOBJ/tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
// #include "utils/glutils.hpp"
#include "utils/timer.hpp"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#define GLM_FORCE_SWIZZLE
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <algorithm>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define __FILEPATH__(x) ((std::string(__FILE__).substr(0, std::string(__FILE__).rfind('/'))+(x)).c_str())

#define deg2rad(x) ((x)*((3.1415926f)/(180.0f)))

#define INIT_WIDTH 1600
#define INIT_HEIGHT 900
#define INIT_VIEWPORT_X 0
#define INIT_VIEWPORT_Y 0
#define MAX_BONE_INFLUENCE 4

using namespace glm;
using namespace std;

const char* textureTypes[] = {
    "textureNone",
    "textureDiffuse", 
    "textureSpecular", 
    "textureAmbient", 
    "textureEmissive", 
    "textureHeight", 
    "textureNormal", 
    "textureShininess", 
    "textureOpacity", 
    "textureDisplacement", 
    "textureLightmap", 
    "textureReflection", 
    "textureUnknow"
};

const aiTextureType aiTextureTypes[] = {
    aiTextureType_NONE, 
    aiTextureType_DIFFUSE, 
    aiTextureType_SPECULAR, 
    aiTextureType_AMBIENT, 
    aiTextureType_EMISSIVE, 
    aiTextureType_HEIGHT, 
    aiTextureType_NORMALS, 
    aiTextureType_SHININESS, 
    aiTextureType_OPACITY, 
    aiTextureType_DISPLACEMENT, 
    aiTextureType_LIGHTMAP, 
    aiTextureType_REFLECTION, 
    aiTextureType_UNKNOWN
};

// Print OpenGL context related information.
void dumpInfo(void)
{
	printf("Vendor: %s\n", glGetString (GL_VENDOR));
	printf("Renderer: %s\n", glGetString (GL_RENDERER));
	printf("Version: %s\n", glGetString (GL_VERSION));
	printf("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
}

void shaderLog(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		printf("%s\n", errorLog);
		delete[] errorLog;
	}
}

void printGLError()
{
    GLenum code = glGetError();
    switch(code)
    {
    case GL_NO_ERROR:
        std::cout << "GL_NO_ERROR" << std::endl;
        break;
    case GL_INVALID_ENUM:
        std::cout << "GL_INVALID_ENUM" << std::endl;
        break;
    case GL_INVALID_VALUE:
        std::cout << "GL_INVALID_VALUE" << std::endl;
        break;
    case GL_INVALID_OPERATION:
        std::cout << "GL_INVALID_OPERATION" << std::endl;
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
        break;
    case GL_OUT_OF_MEMORY:
        std::cout << "GL_OUT_OF_MEMORY" << std::endl;
        break;
    case GL_STACK_UNDERFLOW:
        std::cout << "GL_STACK_UNDERFLOW" << std::endl;
        break;
    case GL_STACK_OVERFLOW:
        std::cout << "GL_STACK_OVERFLOW" << std::endl;
        break;
    default:
        std::cout << "GL_ERROR" << std::endl;
    }
}

