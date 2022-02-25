#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "ray.h"
#include "vec3.h"
#include "intersect.h"
#include "texture.h"

/*
    class：material 材质基类
    function：
    1、 生成散射后的光线（即吸收入射光线后的散射）
    2、 如果散射，散射后光线的变暗情况attenuate
*/

struct hitRecord;

class material {
public:
    virtual bool scatter (
        const ray& r_in, const hitRecord& rec, color& attenuation, ray& scattered
    ) const = 0;

    virtual color emitted(double u, double v, const point3& p) const {
        return color(0, 0, 0);
    }
};

class diffuse_light : public material {
public:
    diffuse_light(shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

    virtual bool scatter(
        const ray& r_in, const hitRecord& rec, color& attenuation, ray& scattered
    ) const override {
        return false;
    }

    virtual color emitted(double u, double v, const point3& p) const override {
        return emit->value(u, v, p);
    }

public:
    shared_ptr<texture> emit;
};



#endif