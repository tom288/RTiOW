#ifndef RAY_H_
#define RAY_H_

#include <glm/glm.hpp>

class Ray
{ public:

    Ray(const glm::dvec3& org, const glm::dvec3& dir) : org(org), dir(dir) { }

    glm::dvec3 at(double t) const
    {
        return org + t * dir;
    }

    glm::dvec3 org;
    glm::dvec3 dir;
};

#endif
