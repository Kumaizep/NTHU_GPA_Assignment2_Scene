#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "common.h"
#include "shader.hpp"


class Texture
{
public:
    GLuint id;
    string type;
    string path;
    int width;
    int height;

	Texture(const string &filepath, string typeName)
	{
		id = loadTexture(filepath, width, height);
        type = typeName;
        path = filepath;
	}

	GLuint loadTexture(const string &path, int &width, int &height)
    {
        int colorChannel;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &colorChannel, 0);

        GLuint textureID;
        glGenTextures(1, &textureID);

        if (data)
        {
            GLint internalFormat;
            GLint format;
            switch (colorChannel) {
                case 1:
                    internalFormat = GL_R8;
                    format = GL_RED;
                    break;
                case 2:
                    internalFormat = GL_RG8;
                    format = GL_RG;
                    break;
                case 3:
                    internalFormat = GL_RGB8;
                    format = GL_RGB;
                    break;
                case 4:
                    internalFormat = GL_RGBA8;
                    format = GL_RGBA;
                    break;
                default:
                    cout << "ERROR::Texture with unknow color channel " << colorChannel << ": " << path << endl;
                    break;
            }

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            std::cout << "ERROR::Texture failed to load at path: " << path << std::endl;
        }

        stbi_image_free(data);
        return textureID;
    }

    int comparePath(const char* filepath)
	{
		return strcmp(path.data(), filepath);
	}

	void activeAndBind(Shader& shader, GLuint unit)
	{
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, id);
        shader.setVec2("textureSizeReciprocal", 1.0f / width, 1.0f / height);
	}
	
};

#endif
