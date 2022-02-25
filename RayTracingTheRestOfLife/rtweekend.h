//rtweekend.h
#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <functional>
#include <random>
// Common Headers

//#include "ray.h"
//#include "vec3.h"

// Usings

using std::shared_ptr;
using std::make_shared;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

    /*
    * ���ߺ���
    */

//�Ƕ�ת����

inline double degrees_to_radians(double degrees) 
{
    return degrees * pi / 180;
}

inline double ffmin(double a, double b) 
{ 
    return a <= b ? a : b; 
}
inline double ffmax(double a, double b) 
{ 
    return a >= b ? a : b; 
}

// Random Generation
/*
inline double random_double() 
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    static std::function<double()> rand_generator =
        std::bind(distribution, generator);
    return rand_generator();
}
*/

//�������һ��ʵ��

inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

//�������һ����Χ�ڵ�ʵ��

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

// ����x��һ����Χ��

inline double clamp(double x, double min, double max) 
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

//�������һ����Χ�ڵ�����

inline int random_int(int min, int max) {
    return static_cast<int>(random_double(min, max + 1));
}

#endif