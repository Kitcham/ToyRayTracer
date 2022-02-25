#ifndef RAY_H
#define RAY_H

#include "vec3.h"

/*
    class��ray ���ߣ����ߣ���
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

        point3 origin() const { return orig; }    // ����ԭ��
        vec3 direction() const { return dir; }    // ���߷���
        double time() const { return tm; }        // ����ʱ��
         
        point3 at(double t) const                 // ����t��ĳ��λ�õĶ�Ӧ��
        {
            return orig + t * dir;
        }

    public:
        point3 orig; // ��Դ����
        vec3 dir;    // ���շ���
        double tm;   // ����ʱ��
};

#endif