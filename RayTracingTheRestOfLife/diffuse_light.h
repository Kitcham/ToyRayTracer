#ifndef DIFFUSE_LIGHT_H
#define DIFFUSE_LIGHT_H

#include "rtweekend.h"
#include "ray.h"
#include "vec3.h"
#include "intersect.h"
#include "material.h"

/*
    Class:diffuse_light 漫反射灯
    inherit：material
*/

class diffuse_light : public material {
public:
    diffuse_light(shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

    virtual bool scatter(
        const ray& r_in, const hitRecord& rec, color& attenuation, ray& scattered
    ) const {
        return false;
    }
    // 天花板上的灯周围的噪声是由于灯是双面的
    // 灯和天花板之间有一个很小的空间。
    // 要让仅光向下发射，可以通过让intersect发出的成员函数获取额外的信息来做到
    virtual color emitted(const ray& r_in, const hitRecord& rec, double u, double v, const point3& p
    ) const override {
        if (rec.frontFace)
        {
            return emit->value(u, v, p);
        }
        else
        {
            return color(0, 0, 0);
        }


    }

public:
    shared_ptr<texture> emit;
};

#endif