#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
using namespace std;
#include "global.hpp"
#include "shader.hpp"
#include "camera.hpp"

// Keyboard input callback
void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(win, GL_TRUE);
    }
}

// Mouse cursor movement callback
double xold, yold;
void cursorPosCallback(GLFWwindow* win, double xpos, double ypos)
{
    xold = xpos;
    yold = ypos;
}

// Mouse button input callback
void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods)
{
    if (action == GLFW_PRESS) { }
}

// Mouse scroll wheel movement callback
void scrollCallback(GLFWwindow* win, double xoffset, double yoffset) { }

// Creates and returns a window
GLFWwindow* makeWindow(const char* title)
{
    GLFWwindow* win = nullptr;
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Configure antialiasing
    if (AA_X > 1) glfwWindowHint(GLFW_SAMPLES, AA_X);

#ifdef __APPLE__
    // Fixes compilation on OS X
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if (FULLSCREEN)
    {
        GLFWmonitor* mon = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(mon);
        win = glfwCreateWindow(mode->width, mode->height, title, mon, nullptr);
    }
    else win = glfwCreateWindow(WIN_W, WIN_H, title, nullptr, nullptr);

    glfwMakeContextCurrent(win);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Register callbacks
    glfwSetKeyCallback(win, keyCallback);
    glfwSetMouseButtonCallback(win, mouseButtonCallback);
    glfwSetCursorPosCallback(win, cursorPosCallback);
    glfwSetScrollCallback(win, scrollCallback);

    // Ignore hidden triangles
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // Configure vertical sync
    if (!VSYNC) glfwSwapInterval(0);

    // Configure antialiasing
    if (AA_X > 1) glEnable(GL_MULTISAMPLE);

    if (RAW_INPUT && glfwRawMouseMotionSupported())
    {
        // Disable mouse acceleration and scaling
        glfwSetInputMode(win, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    glewExperimental = GL_TRUE;
    glewInit();

    int width, height;
    glfwGetFramebufferSize(win, &width, &height);
    glViewport(0, 0, width, height);

    return win;
}

// Launches the program
int main()
{
    GLFWwindow* win = makeWindow(":D");
    if (!win)
    {
        glfwTerminate();
        return 1;
    }

    Shader shader("basic");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glfwGetCursorPos(win, &xold, &yold);
    size_t frames = 0;
    double deltaTime, oldTime = glfwGetTime(), elapsed = 0;

    while (!glfwWindowShouldClose(win))
    {
        deltaTime = glfwGetTime() - oldTime;
        oldTime += deltaTime;
        elapsed += deltaTime;

        // Runs every 1 second on average
        if (elapsed > 1 - elapsed / ++frames / 2)
        {
            cout << "T = " << 1000.0 * elapsed / frames << " ms\t"
               << "FPS = " << frames / elapsed << endl;
            elapsed = 0;
            frames = 0;
        }

        glm::dvec3 input;
        input.x = glfwGetKey(win, GLFW_KEY_D)
                - glfwGetKey(win, GLFW_KEY_A);
        input.y = glfwGetKey(win, GLFW_KEY_SPACE)
               - (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT)
               || glfwGetKey(win, GLFW_KEY_LEFT_CONTROL)
               || glfwGetKey(win, GLFW_KEY_CAPS_LOCK));
        input.z = glfwGetKey(win, GLFW_KEY_W)
                - glfwGetKey(win, GLFW_KEY_S);
        // TODO

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        // TODO

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
