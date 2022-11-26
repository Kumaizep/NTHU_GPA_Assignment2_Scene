#ifndef MESH_HPP
#define MESH_HPP

#include "common.h"
#include "shader.hpp"
#include "texture.hpp"

int checkTexture[14] = {0};

struct Vertex
{
    // position
    vec3 position;
    // normal
    vec3 normal;
    // texCoords
    vec2 texCoords;
    // tangent
    vec3 tangent;
    // bitangent
    vec3 bitangent;
    // bone indexes which will influence this vertex
    int mBoneIDs[MAX_BONE_INFLUENCE];
    // weights from each bone
    float mWeights[MAX_BONE_INFLUENCE];
};

class Mesh
{
public:
    vector<Vertex>  vertices;
    vector<GLuint>  indices;
    vector<Texture> textures;

    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
        : vertices(vertices), indices(indices), textures(textures)
    {
        setMesh();
    }

    void draw(Shader& shader) 
    {
        GLuint textureTypeNumber[13];
        for (int i = 0; i < 13; ++i)
        {
            textureTypeNumber[i] = 1;
        }
        for (GLuint i = 0; i < textures.size(); i++)
        {
            textures[i].activeAndBind(shader, i);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

private:
    GLuint VAO, VBO, EBO;
    void setMesh()
    {
        // cout << "222 ";
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); 
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
            (GLvoid*)0);

        glEnableVertexAttribArray(1); 
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
            (GLvoid*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2); 
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
            (GLvoid*)offsetof(Vertex, texCoords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
            (GLvoid*)offsetof(Vertex, tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
            (GLvoid*)offsetof(Vertex, bitangent));

        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex),
            (GLvoid*)offsetof(Vertex, mBoneIDs));

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (GLvoid*)offsetof(Vertex, mWeights));

        glBindVertexArray(0);
    }
};

#endif