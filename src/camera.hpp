#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "config.hpp"
#include "ray.hpp"

// Constants
static const glm::dvec3 UP(0.0, 1.0, 0.0);
static const double PITCH_MAX = 89.0;
static const double FOV_MIN = 10.0;
static const double FOV_MAX = 170.0;
static const double NEAR = 0.1;
static const double FAR = 1000.0;
static const double ACC = 200.0;
static const double SPEED_MAX = 20.0;
static const double FRICTION = 0.0001;

class Camera
{ public:

    Camera(const glm::dvec3& position, const glm::dvec3& lookAt) : position(position)
    {
        const double theta = glm::radians(double(VFOV));
        const double height = glm::tan(theta / 2.0) * 2.0;
        const double width = height * WIN_W / WIN_H;

        look = glm::normalize(position - lookAt);
        right = glm::normalize(glm::cross(UP, look));
        above = glm::normalize(glm::cross(look, right));

        zont = width * right;
        vert = height * above;
        lowerLeft = position - (zont + vert) / 2.0 - look;
    }

    Ray getRay(const double& u, const double& v) const
    {
        return Ray(position, lowerLeft + u * zont + v * vert - position);
    }

private:

    // Kinematics
    glm::dvec3 position;
    glm::dvec3 velocity;

    // Direction
    glm::dvec3 look;
    glm::dvec3 right;
    glm::dvec3 above;

    // Raytracing
    glm::dvec3 lowerLeft;
    glm::dvec3 zont;
    glm::dvec3 vert;
};

#endif
