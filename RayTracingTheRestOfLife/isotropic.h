#ifndef ISOTROPIC_H
#define ISOTROPIC_H

#include "rtweekend.h"
#include "ray.h"
#include "vec3.h"
#include "intersect.h"
#include "material.h"
#include "texture.h"

class isotropic : public material {
public:
    isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
    isotropic(shared_ptr<texture> a) : albedo(a) {}

    /* PDF修复前 无效 无法call
    virtual bool scatter(
        const ray& r_in, const hitRecord& rec, color& attenuation, ray& scattered
    ) const {
        //scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
        scattered = ray(rec.p, random_unit_vector(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }
    */

    bool scatter(
        const ray& r_in, const hitRecord& rec, scatterRecord& srec
    ) const override {
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = make_shared<sphere_pdf>();
        srec.isSpecular = false;
        return true;
    }

    double scattering_pdf(
        const ray& r_in, const hitRecord& rec, const ray& scattered
    ) const override {
        return 1 / (4 * pi);
    }

public:
    shared_ptr<texture> albedo;
};

#endif