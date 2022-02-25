#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "rtweekend.h"
#include "ray.h"
#include "vec3.h"
#include "intersect.h"
#include "material.h"
#include "texture.h"

/*
    class：lambertian 朗伯体（完全漫反射体）
    inherit：material
*/

class lambertian : public material {
public:
    lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
    lambertian(shared_ptr<texture> a) : albedo(a) {}

    virtual bool scatter(
        const ray& r_in, const hitRecord& rec, color& attenuation, ray& scattered
    ) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        // attenuation光线衰减率，即每次散射后，光线的衰减情况
        // 要么是光线永远发生散射, 每次散射衰减至R, 要么是光线并不衰减, 转而物体吸收(1-R)的光线
        // Albedo描述了光线击中表面后会呈现一种怎样的状态，若光线没有散射，光线即消失
        // 同时需要保证散射光线的时间与射入光线的时间一致
        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<texture> albedo;
};

#endif