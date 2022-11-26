#ifndef MODEL_HPP
#define MODEL_HPP

#include "mesh.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "texture.hpp"

vector<Texture> loadedTextures;

struct ImageData
{
    int width;
    int height;
    GLenum format;
    unsigned char* data;

    ImageData() : width(0), height(0), data(0) {}
};


class Model 
{
public:
    Model(const string path)
    {
        loadModel(path);
    }

    void draw(Shader& shader)
    {
        // cout << "DEBUG::MODEL::C-MODEL-F-D: " << meshes.size() << endl;
        for (GLuint i = 0; i < meshes.size(); i++)
            meshes[i].draw(shader);
    }
private:
    vector<Mesh> meshes;
    string directory;

    void loadModel(const string path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, 
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace); 

        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        {
            cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene)
    {
        for (GLuint i = 0; i < node->mNumMeshes; i++)
        {
            meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene)); 
        }
        for (GLuint i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        vector<Vertex> vertices  = processVertices(mesh);
        vector<GLuint> indices   = processIndices(mesh);
        vector<Texture> textures = processTextures(mesh, scene);
        return Mesh(vertices, indices, textures);
    }

    vector<Vertex> processVertices(aiMesh* mesh)
    {
        vector<Vertex> vertices;
        for (GLuint i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            vertex.position = vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            if (mesh->HasNormals())
            {
                vertex.normal = vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            }
            if (mesh->mTextureCoords[0])
            {
                vertex.texCoords = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                vertex.tangent   = vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                vertex.bitangent = vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
            }
            else
            {
                vertex.texCoords = vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }
        return vertices;
    }

    vector<GLuint> processIndices(aiMesh* mesh)
    {
        vector<GLuint> indices;
        for (GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            for (GLuint j = 0; j < (mesh->mFaces[i]).mNumIndices; j++)
            {
                indices.push_back((mesh->mFaces[i]).mIndices[j]);
            }
        }
        return indices;
    }

    vector<Texture> processTextures(aiMesh* mesh, const aiScene* scene)
    {
        vector<Texture> textures;
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        for (int i = 1; i < 13; ++i)
        {
            vector<Texture> Maps = loadMaterialTextures(material, aiTextureTypes[i], textureTypes[i]);
            textures.insert(textures.end(), Maps.begin(), Maps.end());
        }

        return textures;
    }


    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        // cout << "DEBUG::MODEL::C-MODLE-F-LMT::TP: " << typeName << endl;
        // cout << "DEBUG::MODEL::C-MODLE-F-LMT::GTC: " << mat->GetTextureCount(type) << endl;
        for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            string filepath = directory + "/" + str.C_Str();
            int loadIndex = getLoadedTextureId(filepath);
            if (loadIndex == -1)
            {
                cout << "DEBUG::MODEL::C-MODLE-F-LMT::FN: " << str.C_Str() << endl;
                Texture texture = Texture(filepath, typeName);
                textures.push_back(texture);
                loadedTextures.push_back(texture);
            }
            else
            {
                textures.push_back(loadedTextures[loadIndex]);
            }
        }
        return textures;
    }

    int getLoadedTextureId(string str)
    {
        for(GLuint j = 0; j < loadedTextures.size(); j++)
        {
            if(loadedTextures[j].comparePath(str.c_str()) == 0)
            {
                return j;
            }
        }
        return -1;
    }
};

#endif