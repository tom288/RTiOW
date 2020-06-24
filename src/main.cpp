#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <memory>
#include <math.h>
#include "config.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "geometry.hpp"
#include "utility.hpp"
#include "material.hpp"
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
    // if (AA_X > 1) glfwWindowHint(GLFW_SAMPLES, AA_X);

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
    // if (AA_X > 1) glEnable(GL_MULTISAMPLE);

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

// TODO begin at depth 0 and count up instead
// TODO multiple bounces on hit and lower AA_X for more efficient rendering
glm::dvec3 raycast(const Ray& ray, const Surface& world, int depth)
{
    if (depth <= 0) return glm::dvec3(0.0);
    RayHit hit;

    // A non-zero minimum t value kills shadow acne
    if (world.hit(ray, 0.0001, INF, hit))
    {
        Ray scattered(glm::dvec3(0.0), glm::dvec3(0.0));
        glm::dvec3 atten;
        if (hit.mat->scatter(ray, hit, atten, scattered))
        {
            return atten * raycast(scattered, world, depth - 1);
        }
        return glm::dvec3(0.0);
    }

    double y = glm::normalize(ray.dir).y * 0.5 + 0.5;
    return glm::mix(glm::dvec3(1.0), glm::dvec3(0.5, 0.7, 1.0), y);
}

//
GLubyte* draw()
{
    Camera cam(glm::dvec3(-2,2,1), glm::dvec3(0,0,-1));
    Geometry world;

    world.add(std::make_shared<Sphere>(glm::dvec3(0.0, 0.0, -1.0), 0.5,
              std::make_shared<Diffuse>(glm::dvec3(0.1, 0.2, 0.5))));

    world.add(std::make_shared<Sphere>(glm::dvec3(0.0, -100.5, -1.0), 100,
              std::make_shared<Diffuse>(glm::dvec3(0.8, 0.8, 0.0))));

    world.add(std::make_shared<Sphere>(glm::dvec3(1.0, 0.0, -1.0), 0.5,
              std::make_shared<Metal>(glm::dvec3(0.8, 0.6, 0.2), 0.3)));

    world.add(std::make_shared<Sphere>(glm::dvec3(-1.0, 0.0, -1.0), 0.5,
              std::make_shared<Dielectric>(1.5)));
    world.add(std::make_shared<Sphere>(glm::dvec3(-1.0, 0.0, -1.0), -0.45,
              std::make_shared<Dielectric>(1.5)));

    const int samples = glm::max(1, AA_X);
    const int root = sqrt(samples);

    GLubyte* pixels = new GLubyte[WIN_W * WIN_H * 3];
    for (size_t row = 0; row < WIN_H; ++row)
    {
        for (size_t column = 0; column < WIN_W; ++column)
        {
#if 0
            glm::dvec3 color(row / double(WIN_H), column / double(WIN_W), 0.5);
#else
            glm::dvec3 color(0.0);

            for (int s = 0; s < samples; ++s)
            {
                double x;
                double y;
                if (STRATIFY)
                {
                    // TODO Replace this zigzag pattern with a better one
                    x = (0.5 + s) / samples;
                    y = fmod(s, root) / root + (0.5 / samples);
                }
                else
                {
                    x = random();
                    y = random();
                }
                double u = (column + x) / double(WIN_W);
                double v = (row + y) / double(WIN_H);
                Ray ray = cam.getRay(u, v);
                color += raycast(ray, world, RAY_DEPTH);
            }

            color /= samples;
            // Gamma correction
            color = glm::sqrt(color);
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
