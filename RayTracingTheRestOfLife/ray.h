#ifndef RAY_H
#define RAY_H

#include "vec3.h"

/*
    class：ray 视线（光线）类
*/

class ray 
{
    public:
        ray() {}
        ray(const point3& origin, const vec3& direction)
            : orig(origin), dir(direction) {}
        
        ray(const point3& origin, const vec3& direction, double time = 0.0)
            : orig(origin), dir(direction), tm(time)
        {}

        point3 origin() const { return orig; }    // 光线原点
        vec3 direction() const { return dir; }    // 光线方向
        double time() const { return tm; }        // 光线时刻
         
        point3 at(double t) const                 // 根据t在某个位置的对应点
        {
            return orig + t * dir;
        }

    public:
        point3 orig; // 光源坐标
        vec3 dir;    // 光照方向
        double tm;   // 光线时刻
};

#endif