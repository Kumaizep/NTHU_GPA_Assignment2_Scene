#include "../include/common.h"
#include "../include/camera.hpp"
#include "../include/shader.hpp"
#include "../include/model.hpp"
#include "../include/frame.hpp"
#include <vector>

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

int guiMenuWidth = INIT_WIDTH;
int frameWidth = INIT_WIDTH;
int frameHeight = INIT_HEIGHT;

int outputMode = 0;
int filterMode = 0;
int testMode = 0;

bool compareBarEnable = false;
float compareBarX = INIT_WIDTH / 2.0f;
bool compareBarMoveEnable = false;

vec2 magnifierCenter = vec2(frameWidth, frameHeight) / 2.0f;
float magnifierRadius = 70.0f;
bool magnifierResizeEnable = false;
bool magnifierMoveEnable = false;
vec2 magnifierMoveOffset = vec2(0.0f);

bool needUpdateFBO = false;

vector<Model> models;

const char* filterTypes[] = {
    "Default",
    "Image Abstraction",
    "Watercolor",
    "Magnifier", 
    "Bloom Effect", 
    "Pixelization", 
    "Sine Wave"
};

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

void processCompareBarMove(GLFWwindow *window)
{   
    if (!compareBarMoveEnable)
        return;
    double x, y;
    glfwGetCursorPos(window, &x, &y); 
    
    if (x > 30 && x < frameWidth - 30)
        compareBarX = x;
}

void processMagnifierResize(GLFWwindow *window)
{   
    if (!magnifierResizeEnable)
        return;
    double x, y;
    glfwGetCursorPos(window, &x, &y); 
    
    // cout << "DEBUG::MAIN::PMR:1" << endl;
    if (y > 30 && y < frameHeight - 30 && magnifierCenter.y - (frameHeight - y) > 30.0f)
    {
        // cout << "DEBUG::MAIN::PMR:2" << endl;
        magnifierRadius = magnifierCenter.y - (frameHeight - y);
    }
}

void processMagnifierMove(GLFWwindow *window)
{   
    if (!magnifierMoveEnable)
        return;
    double x, y;
    glfwGetCursorPos(window, &x, &y); 

    if (x > 30 && x < frameWidth - 30 && y > 30 && y < frameHeight - 30)
        magnifierCenter = vec2(x, frameHeight - y) + magnifierMoveOffset;
}

void updateFrameVariable(Frame& frame)
{
    frame.setTestMode(testMode);
    frame.setFilterMode(filterMode);
    frame.setFrameSize(frameWidth, frameHeight);
    frame.setCompareBarEnable(compareBarEnable);
    frame.setCompareBarX(compareBarX);
    frame.setMagnifierCeanter(magnifierCenter);
    frame.setMagnifierRadius(magnifierRadius);
    if (needUpdateFBO)
    {
        frame.updateFrameBufferObject();
        needUpdateFBO = false;
    }
}

void display(Shader& shader, Camera& camera)
{
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

void windowUpdate(Shader& frameShader, Shader& shader, Camera& camera, Frame& frame)
{
    updateFrameVariable(frame);

    // Update to Frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, frame.FBO);
    glClearColor(0.0f, 0.25f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
    glEnable(GL_DEPTH_TEST);
    display(shader, camera);

    // Update to window
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    frame.draw(frameShader);
}

void reshapeResponse(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
    compareBarX = compareBarX / frameWidth * width;
    frameWidth = guiMenuWidth = width;
    frameHeight = height;
    needUpdateFBO = true;
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
        case GLFW_KEY_G:
            if (action == GLFW_PRESS) testMode = (testMode + 1) % 2;
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
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS) {
            if (filterMode == 3)
            {
                // cout <<"DEBUG::MAIN::MR::3: " << length(vec2(x,frameHeight - y) - magnifierCenter) << " " << length(vec2(x,frameHeight - y) - (magnifierCenter - vec2(0.0f, magnifierRadius + 1))) << endl;
                if(length(vec2(x,frameHeight - y) - (magnifierCenter - vec2(0.0f, magnifierRadius + 1))) < 8)
                {
                    cout <<"DEBUG::MAIN::MR::magnifierResizeEnabled" << endl;
                    magnifierResizeEnable = true;
                }
                else if (length(vec2(x,frameHeight - y) - magnifierCenter) < magnifierRadius)
                {
                    cout <<"DEBUG::MAIN::MR::magnifierMoveEnabled" << endl;
                    magnifierMoveEnable = true;
                    magnifierMoveOffset = magnifierCenter - vec2(x,frameHeight - y);
                }
            }
            else if (compareBarEnable && filterMode != 0)
            {
                cout <<"DEBUG::MAIN::MR::1:" << x - compareBarX << " " << y - (frameHeight / 2.0f - 5.0f) << endl;
                if(abs(x - compareBarX) <= 6 && abs(frameHeight - y - (0.5f * frameHeight)) <= 20)
                {
                    cout <<"DEBUG::MAIN::MR::compareBarEnabled" << endl;
                    compareBarMoveEnable = true;
                }
            }
            printf("Mouse %d is pressed at (%f, %f)\n", button, x, y);
        }
        else if (action == GLFW_RELEASE) {
            compareBarMoveEnable = false;
            magnifierResizeEnable = false;
            magnifierMoveEnable = false;
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

void guiMenu()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(guiMenuWidth + 2, 0));
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
        if (ImGui::BeginMenu("FrameFilter"))
        {
            for (int i = 0; i < 7; ++i)
            {
                if (filterMode == i)
                {
                    ImGui::TextDisabled(("＞　" + string(filterTypes[i])).c_str());
                }
                else if (ImGui::MenuItem(("　　" + string(filterTypes[i])).c_str()))
                {
                    filterMode = i;
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("CompareBar"))
        {
            if (!compareBarEnable)
            {
                ImGui::TextDisabled("＞　Disabled");
                if (ImGui::MenuItem("　　Enable"))
                {
                    compareBarEnable = true;
                }
            }
            else
            {
                if (ImGui::MenuItem("　　Disable"))
                {
                    compareBarEnable = false;
                }
                ImGui::TextDisabled("＞　Enabled");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("ControlHelp"))
        {
            ImGui::Text("　Keyboard:　");
            ImGui::Text("　　　W/A/S/D:　");
            ImGui::Text("　　　　　 Move forward/left/backward/right　");
            ImGui::Text("　　　Z/X:　");
            ImGui::Text("　　　　　Adjust the eye height up/down　");
            ImGui::Text("　Mouse:　");
            ImGui::Text("　　　Right button:　");
            ImGui::Text("　　　　　Click menu and control compare-bar/magnifier　");
            ImGui::Text("　　　　　Compare-bar:　");
            ImGui::Text("　　　　　　　Drag the grey bar to adjust the position　");
            ImGui::Text("　　　　　Magnifier:　");
            ImGui::Text("　　　　　　　Drag the grey dot to resize magnifier　");
            ImGui::Text("　　　　　　　Drag the inside to adjust the position　");
            ImGui::Text("　　　Middle button:　");
            ImGui::Text("　　　　　Drag anywhere");
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
    Shader frameShader("asset/frameVertex.vs.glsl", "asset/frameFragment.fs.glsl");
    Camera camera = Camera()
                        .withPosition(vec3(0.0f, 125.0f, 0.0f))
                        .withFar(5000.0f)
                        .withMoveSpeed(300.0f)
                        .withTheta(180.0f);
    cout << "DEBUG::MAIN::C-CAMERA-F-GV: " << camera.front.x << " " << camera.front.y << " " << camera.front.z << endl;
    Frame frame = Frame();
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
        processCompareBarMove(window);
        processMagnifierResize(window);
        processMagnifierMove(window);
        windowUpdate(frameShader, shader, camera, frame);
        guiMenu();

        // swap buffer from back to front
        glfwSwapBuffers(window);
    }

    menuCleanup();
    // just for compatibiliy purposes
    return 0;
}
