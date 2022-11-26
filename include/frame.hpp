#ifndef FRAME_HPP
#define FRAME_HPP

#include "common.h"
#include "texture.hpp"
#include "shader.hpp"

const GLfloat quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    }; 

class Frame
{
public:
    GLuint FBO;

    Frame()
    {
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        
        createFrameTextureObject();

        createFrameRenderObject();

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "ERROR::FRAME::INIT: Framebuffer is not complete!" << endl;
        else
            cout << "DEBUG::FRAME::INIT: Framebuffer is complete!" << endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glDeleteFramebuffers(1, &FBO);

        createFrameVextexObject();

        filterTextures.push_back(Texture("asset/textures/noise_texture_0001.png", "textureUnknow"));
    }

    void draw(Shader& shader)
    {
        shader.use();

        setupShaderUniform(shader);

        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, FBT);
        int unit = 1;
        for (auto& it: filterTextures) {
            it.activeAndBind(shader, unit++);
        }
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void setTimerCounter(int val)
    {
        timerCounter = val;
    }

    void setFilterMode(int val)
    {
        filterMode = val;
    }

    void setTestMode(int val)
    {
        testMode = val;
    }

    void setFrameSize(int width, int height)
    {
        frameWidth = width;
        frameHeight = height;
    }

    void setCompareBarEnable(bool val)
    {
        compareBarEnbale = val;
    }

    void setCompareBarX(float val)
    {
        compareBarX = val;
    }

    void setMagnifierCeanter(vec2 val)
    {
        magnifierCenter = val;
    }

    void setMagnifierRadius(float val)
    {
        magnifierRadius = val;
    }

    void updateFrameBufferObject()
    {
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        
        createFrameTextureObject();

        createFrameRenderObject();

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "ERROR::FRAME::INIT: Framebuffer is not complete!" << endl;
        else
            cout << "DEBUG::FRAME::INIT: Framebuffer is complete!" << endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

private:
    GLuint FBT;
    GLuint quadVAO;
    vector<Texture> filterTextures;

    int frameWidth = INIT_WIDTH;
    int frameHeight = INIT_HEIGHT;
    
    int timerCounter = 0;
    int filterMode = 0;
    int testMode = 0;

    float compareBarX = (float)INIT_WIDTH / 2;
    bool compareBarEnbale = false;

    vec2 magnifierCenter = vec2(frameWidth, frameHeight) / 2.0f;
    float magnifierRadius = 70.0f;

    void setupShaderUniform(Shader& shader)
    {
        timerCounter = (timerCounter + 1) % 180;
        shader.setInt("timer", timerCounter);
        shader.setInt("testMode", testMode);
        shader.setInt("filterMode", filterMode);
        shader.setBool("compareBarEnable", compareBarEnbale);
        shader.setFloat("compareBarX", compareBarX);
        shader.setVec2("textureSizeReciprocal", 1.0f / (float)frameWidth, 1.0f / (float)frameHeight);
        shader.setVec2("frameSize", (float)frameWidth, (float)frameHeight);
        shader.setVec2("magnifierCenter", magnifierCenter.x, magnifierCenter.y);
        shader.setFloat("magnifierRadius", magnifierRadius);
        // cout << "DEBUG::FRAME::DRAW: " << timerCounter << endl;
        // cout << "DEBUG::FRAME::DRAW: " << frameWidth << " " << frameHeight << endl;
    }

    void createFrameTextureObject()
    {
        glGenTextures(1, &FBT);
        glBindTexture(GL_TEXTURE_2D, FBT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBT, 0);
    }

    void createFrameRenderObject()
    {
        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, frameWidth, frameHeight);  
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    }

    void createFrameVextexObject()
    {
        GLuint quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }
};

#endif