#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "surface.hpp"
#include "utility.hpp"
#include "config.hpp"

// Base
class Material
{ public:

    virtual bool scatter(const Ray& in, const RayHit& hit, glm::dvec3& atten, Ray& scattered) const = 0;
};

// Standard
class Diffuse : public Material
{ public:

    glm::dvec3 albedo;

    Diffuse(const glm::dvec3 albedo) : albedo(albedo) { }

    virtual bool scatter(const Ray& in, const RayHit& hit, glm::dvec3& atten, Ray& scattered) const
    {
        glm::dvec3 bounced;
        if (LAMBERTIAN) bounced = hit.norm + randomUnit();
        else bounced = randomHemi(hit.norm);
        scattered = Ray(hit.point, bounced);
        atten = albedo;
        return true;
    }

    // TODO Could scatter with probability p and have atten be albedo / p
};

// Metallic
class Metal : public Material
{ public:

    glm::dvec3 albedo;
    double fuzz;

    Metal(const glm::dvec3 albedo, const double& fuzz) : albedo(albedo), fuzz(fuzz) { }

    virtual bool scatter(const Ray& in, const RayHit& hit, glm::dvec3& atten, Ray& scattered) const
    {
        glm::dvec3 reflected = glm::reflect(glm::normalize(in.dir), hit.norm);
        scattered = Ray(hit.point, reflected + fuzz * randomUnit());
        atten = albedo;
        return dot(scattered.dir, hit.norm) > 0.0;
    }
};

// Refracts
class Dielectric : public Material
{ public:

    double index;

    Dielectric(double index) : index(index) {}

    virtual bool scatter(const Ray& in, const RayHit& hit, glm::dvec3& atten, Ray& scattered) const
    {
        atten = glm::dvec3(1.0);
        double eta = hit.front ? 1.0 / index : index;
        glm::dvec3 dir = glm::normalize(in.dir);
        double cosTheta = glm::min(-dot(dir, hit.norm), 1.0);
        double sinTheta = glm::sqrt(1.0 - cosTheta * cosTheta);

        double prob = schlick(cosTheta, eta);
        if (eta * sinTheta > 1.0 || random() < prob)
        {
            dir = glm::reflect(dir, hit.norm);
        }
        else
        {
            dir = glm::refract(dir, hit.norm, eta);
        }

        scattered = Ray(hit.point, dir);
        return true;
    }
};

#endif
