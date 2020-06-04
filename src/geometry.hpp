#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <memory>
#include <vector>
#include "surface.hpp"

class Geometry: public Surface
{ public:

    std::vector<std::shared_ptr<Surface>> objects;

    Geometry() {}
    Geometry(std::shared_ptr<Surface> object) { add(object); }

    void clear() { objects.clear(); }
    void add(std::shared_ptr<Surface> object) { objects.push_back(object); }

    bool hit(const Ray& r, double tMin, double tMax, RayHit& hit) const
    {
        RayHit tempHit;
        bool hasHit = false;
        double closest_so_far = tMax;

        for (const auto& object : objects)
        {
            if (object->hit(r, tMin, closest_so_far, tempHit))
            {
                hasHit = true;
                closest_so_far = tempHit.t;
                hit = tempHit;
            }
        }

        return hasHit;
    }
};

#endif
