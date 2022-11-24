#include "Common.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#define INIT_WIDTH 1600
#define INIT_HEIGHT 900
#define INIT_VIEWPORT_X 0
#define INIT_VIEWPORT_Y 0
#define INIT_VIEWPORT_WIDTH 1600
#define INIT_VIEWPORT_HEIGHT 900
#define MAX_BONE_INFLUENCE 4

using namespace glm;
using namespace std;

mat4 view(1.0f);                    // V of MVP, viewing matrix
mat4 projection(1.0f);              // P of MVP, projection matrix

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

GLint um4p;
GLint um4mv;
GLint tex;

GLubyte timerCounter = 0;
bool timerEnabled = true;
float timerCurrent = 0.0f;
float timerLast = 0.0f;
unsigned int timerSpeed = 16;

bool keyPressing[400] = {0};
float keyPressTime[400] = {0.0f};

bool trackballEnable = false;
vec2 mouseCurrent = vec2(0.0f, 0.0f);
vec2 mouseLast = vec2(0.0f, 0.0f);

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

struct Texture
{
    GLuint id;
    string type;
    string path;
};

vector<Texture> loadedTextures;

struct ImageData
{
    int width;
    int height;
    GLenum format;
    unsigned char* data;

    ImageData() : width(0), height(0), data(0) {}
};

enum MoveDirection
{
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT,
    UP,
    DOWN
};

class Camera
{
public:
    // camera status
    vec3 position;
    vec3 front;
    vec3 top;
    vec3 right;
    // perspective status
    float fieldOfView;
    float aspect;
    float near;
    float far;
    // move parameter
    float moveSpeed;
    // trackball parameter
    float trackballSpeed;
    float theta;
    float phi;

    Camera()
    {
        position       = vec3(0.0f, 0.0f, 0.0f);
        front          = vec3(1.0f, 0.0f, 0.0f);
        top            = vec3(0.0f, 1.0f, 0.0f);
        right          = vec3(0.0f, 0.0f, 1.0f);
        fieldOfView    = 60.0f;
        aspect         = (float)INIT_VIEWPORT_WIDTH / (float)INIT_VIEWPORT_HEIGHT;
        near           = 0.1f;
        far            = 1000.0f;
        moveSpeed      = 10.0f;
        trackballSpeed = 0.1f;
        theta          = 0.0f;
        phi            = 0.0f;
    }

    Camera& withPosition(vec3 val)
    {
        position = val;
        return *this;
    }

    Camera& withFront(vec3 val)
    {
        front = val;
        return *this;
    }

    Camera& withTop(vec3 val)
    {
        top = val;
        return *this;
    }

    Camera& withRight(vec3 val)
    {
        right = val;
        return *this;
    }

    Camera& withFieldOfView(float val)
    {
        fieldOfView = val;
        return *this;
    }

    Camera& withAspect(float val)
    {
        aspect = val;
        return *this;
    }

    Camera& withNear(float val)
    {
        near = val;
        return *this;
    }

    Camera& withFar(float val)
    {
        far = val;
        return *this;
    }

    Camera& withMoveSpeed(float val)
    {
        moveSpeed = val;
        return *this;
    }

    Camera& withTrackballSpeed(float val)
    {
        trackballSpeed = val;
        return *this;
    }

    mat4 getPerspective()
    {
        return perspective(radians(fieldOfView), aspect, near, far);
    }

    mat4 getView()
    {
        // cout << "DEBUG::MAIN::C-CAMERA-F-GV: " << position. x << " " << position.y << " " << position.z << endl;
        return lookAt(position, position + front, top);
    }

    void processMove(MoveDirection moveDirction, float timeDifferent)
    {
        float shift = moveSpeed * timeDifferent;
        cout << "DEBUG::MAIN::C-CAMERA-F-PM-1: " << shift << endl;
        if (moveDirction == FORWARD)
            position += front * shift;
        if (moveDirction == BACKWARD)
            position -= front * shift;
        if (moveDirction == LEFT)
            position -= right * shift;
        if (moveDirction == RIGHT)
            position += right * shift;
        if (moveDirction == UP)
            position += top * shift;
        if (moveDirction == DOWN)
            position -= top * shift;
        cout << "DEBUG::MAIN::C-CAMERA-F-PM-2: " << position. x << " " << position.y << " " << position.z << endl;
    }

    void processTrackball(float thetaDifferent, float phiDifferent)
    {
        theta -= thetaDifferent * trackballSpeed;
        phi += phiDifferent * trackballSpeed;
        if (phi > 85)
            phi = 85.0f;
        else if (phi < -85)
            phi += -85.0f;
        updateCameraStatus();
    };

private:
    void updateCameraStatus()
    {
        front.x = cos(radians(theta)) * cos(radians(phi));
        front.y = sin(radians(phi));
        front.z = sin(radians(theta)) * cos(radians(phi));
        front = normalize(front);

        right = normalize(cross(front, vec3(0.0f, 1.0f, 0.0f)));
        top = normalize(cross(right, front));
    }
    
};

class Shader
{
public:
    GLuint program;
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        glViewport(INIT_VIEWPORT_X, INIT_VIEWPORT_Y, INIT_VIEWPORT_WIDTH, INIT_VIEWPORT_HEIGHT);
        glClearColor(0.0f, 0.3f, 0.0f, 1.00f);
        // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // Create Shader Program
        program = glCreateProgram();

        // Create customize shader by tell openGL specify shader type
        char **vertexShaderSource = loadShaderSource(vertexPath);
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
        freeShaderSource(vertexShaderSource);
        glCompileShader(vertexShader);
        shaderLog(vertexShader);

        char **fragmentShaderSource = loadShaderSource(fragmentPath);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
        freeShaderSource(fragmentShaderSource);
        glCompileShader(fragmentShader);
        shaderLog(fragmentShader);

        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        // Tell OpenGL to use this shader program now
        glUseProgram(program);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() 
    { 
        glUseProgram(program); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setInt(const GLchar* name, int value) const
    { 
        glUniform1i(glGetUniformLocation(program, name), value); 
    }

    void setMat4(const GLchar* name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, &mat[0][0]);
    }

private:
    char** loadShaderSource(const char* file)
    {
        FILE* fp = fopen(file, "rb");
        fseek(fp, 0, SEEK_END);
        long sz = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *src = new char[sz + 1];
        fread(src, sizeof(char), sz, fp);
        src[sz] = '\0';
        char **srcp = new char*[1];
        srcp[0] = src;
        return srcp;
    }

    void freeShaderSource(char** srcp)
    {
        delete srcp[0];
        delete srcp;
    }
};

int checkTexture[14] = {0};

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
            glActiveTexture(GL_TEXTURE0 + i);
            string number;
            string name = textures[i].type;
            for (int j = 1; j < 13; ++j)
            {
                if (name == string(textureTypes[j]))
                {
                    number = to_string(textureTypeNumber[j]++);
                    if (checkTexture[j] < i)
                        checkTexture[j] = i;
                }
            }
            // if (i > 0)  
                // cout << name + number << endl;

            glBindTexture(GL_TEXTURE_2D, textures[i].id);
            // shader.setInt((name + number).c_str(), i);
            // glUniform1f(glGetUniformLocation(shader->program, (name + number).c_str()), i);
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

class Model 
{
public:
    Model(const string path)
    {
        loadModel(path);
    }

    void draw(Shader& shader)
    {
        cout << "DEBUG::MAIN::C-MODEL-F-D: " << meshes.size() << endl;
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
        // cout << "DEBUG::MAIN::C-MODLE-F-LMT::TP: " << typeName << endl;
        // cout << "DEBUG::MAIN::C-MODLE-F-LMT::GTC: " << mat->GetTextureCount(type) << endl;
        for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            int loadIndex = getLoadedTextureId(str);
            if (loadIndex == -1)
            {
                cout << "DEBUG::MAIN::C-MODLE-F-LMT::FN: " << str.C_Str() << endl;
                Texture texture;
                texture.id = TextureFromFile(string(str.C_Str()), directory);
                texture.type = typeName;
                texture.path = str.C_Str();
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

    int getLoadedTextureId(aiString str)
    {
        for(GLuint j = 0; j < loadedTextures.size(); j++)
        {
            if(strcmp(loadedTextures[j].path.data(), str.C_Str()) == 0)
            {
                return j;
            }
        }
        return -1;
    }

    unsigned int TextureFromFile(const string &path, const string &directory)
    {
        string filename = directory + '/' + path;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << filename << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    GLint loadTexture(string path, string directory)
    {
        string filename = directory + '/' + path;
        GLuint textureID;
        glGenTextures(1, &textureID);
        ImageData imageData = loadImage(filename.c_str());

        // cout << "DEBUG::MAIN::C-MODLE-F-LT::LoadIMG: " << filename.c_str() << endl;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, imageData.format, imageData.width, imageData.height, 0, 
            imageData.format, GL_UNSIGNED_BYTE, imageData.data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // glBindTexture(GL_TEXTURE_2D, 0);
        return textureID;
    }

    ImageData loadImage(const char* path)
    {
        ImageData texture;
        int n;
        stbi_set_flip_vertically_on_load(true);
        stbi_uc *data = stbi_load(path, &texture.width, &texture.height, &n, 4);
        if(data != NULL)
        {
            if (n == 1)
                texture.format = GL_RED;
            else if (n == 3)
                texture.format = GL_RGB;
            else if (n == 4)
                texture.format = GL_RGBA;

            texture.data = new unsigned char[texture.width * texture.height * 4 * sizeof(unsigned char)];
            memcpy(texture.data, data, texture.width * texture.height * 4 * sizeof(unsigned char));
            stbi_image_free(data);
        }
        else
        {
            cout << "ERROR::Texture failed to load at path: " << path << endl;
        }
        return texture;
    }
};

vector<Model> models;

void initialization()
{
    models.push_back(Model("asset/sponza/sponza.obj"));
    // models.push_back(Model("asset/sibenik/sibenik.obj"));

    timerLast = glfwGetTime();
    mouseLast = vec2(0.0f, 0.0f);   
}

void timerUpdate()
{
    timerLast = timerCurrent;
    timerCurrent = glfwGetTime();
}

void processCameraMove(Camera& camera)
{
    float timeDifferent = 0.0f;
    if (timerEnabled)
        timeDifferent = timerCurrent - timerLast;

    if (keyPressing[GLFW_KEY_W])
        camera.processMove(FORWARD, timeDifferent);
    if (keyPressing[GLFW_KEY_S])
        camera.processMove(BACKWARD, timeDifferent);
    if (keyPressing[GLFW_KEY_A])
        camera.processMove(LEFT, timeDifferent);
    if (keyPressing[GLFW_KEY_D])
        camera.processMove(RIGHT, timeDifferent);
    if (keyPressing[GLFW_KEY_Z])
        camera.processMove(UP, timeDifferent);
    if (keyPressing[GLFW_KEY_X])
        camera.processMove(DOWN, timeDifferent);

}

void processCameraTrackball(Camera& camera, GLFWwindow *window)
{   
    double x, y;
    glfwGetCursorPos(window, &x, &y); 
    mouseLast = mouseCurrent;
    mouseCurrent = vec2(x, y);

    vec2 mouseDifferent = vec2(0.0f, 0.0f);
    if (trackballEnable)
        mouseDifferent = mouseCurrent - mouseLast;

    camera.processTrackball(mouseDifferent.x, mouseDifferent.y);
}

// GLUT callback. Called to draw the scene.
void display(Shader& shader, Camera& camera)
{
    glClearColor(0.0f, 0.25f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (timerEnabled) timerCounter += 1.0f;

    shader.use();

    projection = camera.getPerspective();
    view = camera.getView();
    shader.setMat4("um4p", projection);
    shader.setMat4("um4mv", view);

    for (auto& it : models)
    {
        it.draw(shader);
    }
}

void reshapeResponse(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void keyboardResponse(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_T:
            if (action == GLFW_PRESS) timerEnabled = !timerEnabled;
            break;
        case GLFW_KEY_D:
        case GLFW_KEY_A:
        case GLFW_KEY_W:
        case GLFW_KEY_S:
        case GLFW_KEY_Z:
        case GLFW_KEY_X:
            if (action == GLFW_PRESS)
            {
                keyPressing[key] = true;
                // keyPressTime[key] = timerCounter;
            }
            else if (action == GLFW_RELEASE)
            {
                keyPressing[key] = false;
            }
            break;
        default:
            break;
    }
}

void mouseResponse(GLFWwindow *window, int button, int action, int mods)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y); 
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS) {
            // trackballEnable
            printf("Mouse %d is pressed at (%f, %f)\n", button, x, y);
        }
        else if (action == GLFW_RELEASE) {
            printf("Mouse %d is released at (%f, %f)\n", button, x, y);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        
        if (action == GLFW_PRESS) {
            trackballEnable = true;
            printf("Mouse %d is pressed at (%f, %f)\n", button, x, y);
        }
        else if (action == GLFW_RELEASE) {
            trackballEnable = false;
            printf("Mouse %d is released at (%f, %f)\n", button, x, y);
        }
    }
}

int main(int argc, char **argv)
{
    // initial glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // specifies whether to use full resolution framebuffers on Retina displays
    // glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
    // create window
    GLFWwindow* window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, "GPA_Assignment2", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // glfwSetWindowPos(window, 100, 100);
    glfwMakeContextCurrent(window);
    
    // load OpenGL function pointer
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    dumpInfo();
    Shader shader("asset/vertex.vs.glsl", "asset/fragment.fs.glsl");
    Camera camera = Camera()
                        .withPosition(vec3(0.0f, 120.0f, 0.0f))
                        .withFar(5000.0f)
                        .withMoveSpeed(300.0f);
    initialization();

    // register glfw callback functions
    glfwSetFramebufferSizeCallback(window, reshapeResponse);
    glfwSetKeyCallback(window, keyboardResponse);
    glfwSetMouseButtonCallback(window, mouseResponse);
    
    cout << "DEBUG::MAIN::F-MAIN::1" << endl;
    // main loop
    float timeDifferent = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        // Poll input event

        glfwPollEvents();
        timerUpdate();
        
        processCameraMove(camera);
        processCameraTrackball(camera, window);
        display(shader, camera);

        // swap buffer from back to front
        glfwSwapBuffers(window);
    }

    for (int i = 0; i < 13; ++i)
    {
        cout << i << "\t" << textureTypes[i] << "\t" << checkTexture[i] << endl;
    }
    
    // just for compatibiliy purposes
    return 0;
}
