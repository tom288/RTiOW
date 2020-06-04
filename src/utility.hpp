#ifndef UTILITY_H_
#define UTILITY_H_

#include <limits>
#include <cstdlib>
#include <random>

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

#endif
