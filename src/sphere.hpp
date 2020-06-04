#ifndef SPHERE_H_
#define SPHERE_H_

#include "surface.hpp"

class Sphere: public Surface
{ public:

    glm::dvec3 mid;
    double rad;

    Sphere() { }
    Sphere(glm::dvec3 mid, double rad) : mid(mid), rad(rad) { }

    bool hit(const Ray& ray, double tMin, double tMax, RayHit& hit) const
    {
        glm::dvec3 org = ray.org - mid;
        double a = glm::length2(ray.dir);
        double b = dot(org, ray.dir);
        double c = glm::length2(org) - rad * rad;
        double discriminant = b * b - a * c;

        if (discriminant > 0)
        {
            double root = sqrt(discriminant);
            for (const int& i : {1, -1})
            {
                double temp = (b + root * i) / -a;
                if (temp < tMax && temp > tMin)
                {
                    hit.t = temp;
                    hit.point = ray.at(hit.t);
                    glm::dvec3 outwardNorm = (hit.point - mid) / rad;
                    hit.setNorm(ray, outwardNorm);
                    return true;
                }
            }
        }
        return false;
    }
};

#endif
