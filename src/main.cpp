#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include "global.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "texture.hpp"
#include "ray.hpp"
#include "geometry.hpp"

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
    // glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Register callbacks
    glfwSetKeyCallback(win, keyCallback);
    glfwSetMouseButtonCallback(win, mouseButtonCallback);
    glfwSetCursorPosCallback(win, cursorPosCallback);
    glfwSetScrollCallback(win, scrollCallback);

    // Ignore hidden triangles
    // glEnable(GL_CULL_FACE);
    // glEnable(GL_DEPTH_TEST);

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

double hitSphere(const glm::dvec3& mid, const double& rad, const Ray& ray)
{
    glm::dvec3 org = ray.org - mid;
    double a = glm::length2(ray.dir);
    double b = dot(org, ray.dir);
    double c = glm::length2(org) - rad * rad;
    double discriminant = b * b - a * c;

    if (discriminant < 0) return -1.0;
    else return (b + sqrt(discriminant)) / -a;
}

glm::dvec3 raycast(const Ray& ray)
{
    double t = hitSphere(glm::dvec3(0.0, 0.0, -1.0), 0.5, ray);
    if (t > 0.0)
    {
        glm::dvec3 norm = glm::normalize(ray.at(t) - glm::dvec3(0.0, 0.0, -1.0));
        return glm::dvec3(norm.x + 1.0, norm.y + 1.0, norm.z + 1.0) * 0.5;
    }

    double y = glm::normalize(ray.dir).y * 0.5 + 0.5;
    return glm::mix(glm::dvec3(1.0), glm::dvec3(0.5, 0.7, 1.0), y);
}

//
GLubyte* draw()
{
    double vHeight = 2.0;
    double vWidth = vHeight * WIN_W / WIN_H;
    double fLength = 1.0;

    glm::dvec3 org(0, 0, 0);
    glm::dvec3 horizontal = glm::dvec3(vWidth, 0.0, 0.0);
    glm::dvec3 vertical = glm::dvec3(0.0, vHeight, 0.0);
    glm::dvec3 lowerLeft = org - horizontal / 2.0
                               - vertical   / 2.0
                               - glm::dvec3(0.0, 0.0, fLength);

    GLubyte* pixels = new GLubyte[WIN_W * WIN_H * 3];
    for (size_t row = 0; row < WIN_H; ++row)
    {
        for (size_t column = 0; column < WIN_W; ++column)
        {
#if 0
            glm::dvec3 color(row / double(WIN_H), column / double(WIN_W), 100);
#else
            double u = column / double(WIN_W - 1);
            double v = row / double(WIN_H - 1);
            Ray ray(org, lowerLeft + u * horizontal + v * vertical - org);

            glm::dvec3 color = raycast(ray);
#endif
            color *= 255.999;
            for (size_t c = 0; c < 3; ++c)
            {
                pixels[(row * WIN_W + column) * 3 + c] = color[c];
            }
        }
    }

    return pixels;
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

    Shader shader("textured");
    Texture texture;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glfwGetCursorPos(win, &xold, &yold);
    size_t frames = 0;
    double deltaTime, oldTime = glfwGetTime(), elapsed = 0.0;

    while (!glfwWindowShouldClose(win))
    {
        deltaTime = glfwGetTime() - oldTime;
        oldTime += deltaTime;
        elapsed += deltaTime;

        // Runs every 1 second on average
        if (elapsed > 1 - elapsed / ++frames / 2)
        {
            std::cout << "T = " << 1000.0 * elapsed / frames << " ms\t"
                    << "FPS = " << frames / elapsed << std::endl;
            elapsed = 0.0;
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

        glClear(GL_COLOR_BUFFER_BIT); // | GL_DEPTH_BUFFER_BIT);

        GLubyte* pixels = draw();
        texture.fill(WIN_W, WIN_H, pixels);
        delete[] pixels;

        texture.bind();
        shader.use();
        texture.draw();

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
