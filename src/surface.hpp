#ifndef SURFACE_H_
#define SURFACE_H_

#include "ray.hpp"

struct RayHit
{
    glm::dvec3 point;
    glm::dvec3 norm;
    double t;
    bool front;

    inline void setNorm(const Ray& ray, const glm::dvec3& outwardNorm)
    {
        front = dot(ray.dir, outwardNorm) < 0;
        norm = outwardNorm * (front ? 1.0 : -1.0);
    }
};

class Surface
{ public:

    virtual bool hit(const Ray& ray, double tMin, double tMax, RayHit& hit) const = 0;
};

#endif
