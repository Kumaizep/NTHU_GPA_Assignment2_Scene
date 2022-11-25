#include "../include/common.h"
#include "../include/camera.hpp"
#include "../include/shader.hpp"
#include "../include/model.hpp"

mat4 view(1.0f);                    // V of MVP, viewing matrix
mat4 projection(1.0f);              // P of MVP, projection matrix

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

int outputMode = 0;

vector<Model> models;

void initialization(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("asset/fonts/NotoSansCJK-Medium.ttc", 20.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    
    ImGui::StyleColorsLight();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410 core");

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
    shader.setInt("outputMode", outputMode);

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

bool isInsideRectangle2(vec2 point, vec2 position, vec2 size)
{
    if (point.x > position.x + size.x)
        return false;
    else if (point.x < position.x)
        return false;
    else if (point.y > position.y + size.y)
        return false;
    else if (point.y < position.y)
        return false;
    else
        return true;;
}

void mouseResponse(GLFWwindow *window, int button, int action, int mods)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    // if (!isInsideRectangle2(vec2(x, y), menuPosition, vec2(150, 50)))
    // {
    //     menuEnable = false;
    // }
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
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS) {
            // menuEnable = true;
            // menuPosition = vec2(x, y);
            printf("Mouse %d is pressed at (%f, %f)\n", button, x, y);
        }
        else if (action == GLFW_RELEASE) {
            printf("Mouse %d is released at (%f, %f)\n", button, x, y);
        }
    }
}

void guiMenu()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(INIT_VIEWPORT_WIDTH + 2, 0));
    ImGui::SetNextWindowPos(ImVec2(-1, 0));
    ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("OutputMode"))
        {
            if (outputMode == 0)
            {
                ImGui::TextDisabled("＞　Diffuse Texture");
                if (ImGui::MenuItem("　　Normal Vector"))
                {
                    outputMode = 1;
                }
            }
            else
            {
                if (ImGui::MenuItem("　　Diffuse Texture"))
                {
                    outputMode = 0;
                }
                ImGui::TextDisabled("＞　Normal Vector");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("AnimawwteSakana"))
        {
            // menuEnable = false;
            ImGui::EndMenu();
        }        
        ImGui::EndMenuBar();
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void menuCleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

int main(int argc, char **argv)
{
    // initial glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
                        .withPosition(vec3(0.0f, 125.0f, 0.0f))
                        .withFar(5000.0f)
                        .withMoveSpeed(300.0f)
                        .withTheta(180.0f);
    cout << "DEBUG::MAIN::C-CAMERA-F-GV: " << camera.front.x << " " << camera.front.y << " " << camera.front.z << endl;

    initialization(window);

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
        // cout << "DEBUG::MAIN::C-CAMERA-F-GV: " << camera.front.x << " " << camera.front.y << " " << camera.front.z << endl;

        glfwPollEvents();
        timerUpdate();

        processCameraMove(camera);
        processCameraTrackball(camera, window);
        display(shader, camera);
        guiMenu();

        // swap buffer from back to front
        glfwSwapBuffers(window);
    }

    menuCleanup();

    for (int i = 0; i < 13; ++i)
    {
        cout << i << "\t" << textureTypes[i] << "\t" << checkTexture[i] << endl;
    }
    
    // just for compatibiliy purposes
    return 0;
}
