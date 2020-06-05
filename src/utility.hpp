#ifndef UTILITY_H_
#define UTILITY_H_

#include <limits>
#include <cstdlib>
#include <random>
#include <glm/glm.hpp>

const double INF = std::numeric_limits<double>::infinity();

// Return a double in range 0 <= x < 1
inline double random()
{
    return rand() / (RAND_MAX + 1.0);
}

// Return a double in range min <= x < max
inline double random(double min, double max)
{
    return min + (max - min) * random();
}

// Return a double in range 0 <= x < 1
inline double random_double()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

// Returns a random unit vector
glm::dvec3 randomUnit()
{
    double a = random(0.0, 2.0 * glm::pi<double>());
    double z = random(-1.0, 1.0);
    double r = glm::sqrt(1.0 - z * z);
    return glm::dvec3(r * glm::cos(a), r * glm::sin(a), z);
}

// Returns a random unit vector in a hemisphere
glm::dvec3 randomHemi(const glm::dvec3& norm)
{
    glm::dvec3 unit = randomUnit();
    if (glm::dot(unit, norm) < 0.0) unit *= -1.0;
    return unit;
}

#endif
