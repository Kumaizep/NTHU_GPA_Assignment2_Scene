#include "Common.h"

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

using namespace glm;
using namespace std;

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

void My_Init()
{
	glClearColor(0.0f, 0.6f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (timer_enabled) timer_cnt += 1.0f;
}

void My_Reshape(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void My_Keyboard(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_T:
            if (action == GLFW_PRESS) timer_enabled = !timer_enabled;
            break;
        case GLFW_KEY_F1:
            if (action == GLFW_PRESS) printf("F1 is pressed\n");
            break;
        case GLFW_KEY_PAGE_UP:
            if (action == GLFW_PRESS) printf("Page up is pressed\n");
            break;
        case GLFW_KEY_LEFT:
            if (action == GLFW_PRESS) printf("Left arrow is pressed\n");
            break;
        default:
            break;
    }
}

void My_Mouse(GLFWwindow *window, int button, int action, int mods)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            printf("Mouse %d is pressed at (%f, %f)\n", button, x, y);
        }
        else if (action == GLFW_RELEASE) {
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
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // fix compilation on OS X
#endif

    // specifies whether to use full resolution framebuffers on Retina displays
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
    // create window
    GLFWwindow* window = glfwCreateWindow(600, 600, "AS2_Framework", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetWindowPos(window, 100, 100);
    glfwMakeContextCurrent(window);
    
    // load OpenGL function pointer
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    dumpInfo();
    My_Init();

    // register glfw callback functions
    glfwSetFramebufferSizeCallback(window, My_Reshape);
    glfwSetKeyCallback(window, My_Keyboard);
    glfwSetMouseButtonCallback(window, My_Mouse);
    
    // main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll input event
        glfwPollEvents();
        
        My_Display();

        // swap buffer from back to front
        glfwSwapBuffers(window);
    }
    
    // just for compatibiliy purposes
    return 0;
}
