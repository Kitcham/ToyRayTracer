#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "rtweekend.h"
#include "ray.h"
#include "vec3.h"
#include "intersect.h"

#include "material.h"
#include "texture.h"
#include "ONB.h"
#include "PDF.h"

/*
    class：lambertian 朗伯体（完全漫反射体）
    inherit：material
*/

class lambertian : public material {
public:
    //lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
    //lambertian(shared_ptr<texture> a) : albedo(a) {}

    lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
    lambertian(shared_ptr<texture> a) : albedo(a) {}

    virtual bool scatter(
        //const ray& r_in, const hitRecord& rec, color& attenuation, ray& scattered
        const ray& r_in, const hitRecord& rec, color& alb, ray& scattered, double& pdf
    ) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
        {
            scatter_direction = rec.normal;
        }
        onb uvw;
        uvw.build_from_w(rec.normal);
        auto direction = uvw.local(random_cosine_direction());

        scattered = ray(rec.p, unit_vector(direction), r_in.time());
        alb = albedo->value(rec.u, rec.v, rec.p);
        /*
        // 构建一个向光源发射更多光线的PDF，方向球上的MC集成
        // ∫f(x)≈f(r)/p(r)
        // 对朗伯体，p(direction)=cos(θ)/Π
        //pdf = dot(rec.normal, scattered.direction()) / pi;
        */

        /*
        pdf = 0.5 / pi;   // 随机半球采样
        */
        pdf = dot(uvw.w(), scattered.direction()) / pi;
        return true;
        
        /*
        // attenuation光线衰减率，即每次散射后，光线的衰减情况
        // 要么是光线永远发生散射, 每次散射衰减至R, 要么是光线并不衰减, 转而物体吸收(1-R)的光线
        // Albedo描述了光线击中表面后会呈现一种怎样的状态，若光线没有散射，光线即消失
        // 同时需要保证散射光线的时间与射入光线的时间一致
        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        */
        
    }

    double scattering_pdf(
        const ray& r_in, const hitRecord& rec, const ray& scattered
    ) const override {
        auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
        return cosine < 0 ? 0 : cosine / pi;
    }

public:
    shared_ptr<texture> albedo;
};

#endif