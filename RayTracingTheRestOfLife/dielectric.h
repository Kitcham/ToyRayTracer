#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "rtweekend.h"
#include "ray.h"
#include "vec3.h"
#include "intersect.h"
#include "material.h"

/*
    Class:dielectric 绝缘体
    inherit：material
*/

class dielectric : public material {
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}
    /*使用pdf前
    virtual bool scatter(
        const ray& r_in, const hitRecord& rec, color& attenuation, ray& scattered
    ) const {
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.frontFace ? (1.0 / ir) : ir;

        
        vec3 unit_direction = unit_vector(r_in.direction());
        
        //vec3 refracted = refract(unit_direction, rec.normal, refraction_ratio);

        //scattered = ray(rec.p, refracted);

        double cos_theta = ffmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        // 不可折射时，Sniff方程无解，认为发生了反射
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
        {
            direction = reflect(unit_direction, rec.normal);
        }
            
        else
        {
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }
            

        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }
    */

    virtual bool scatter(
        const ray& r_in, const hitRecord& rec, scatterRecord& srec
    ) const override {
        srec.isSpecular = true;
        srec.pdf_ptr = nullptr;
        srec.attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.frontFace ? (1.0 / ir) : ir;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        // 不可折射时，Sniff方程无解，认为发生了反射
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        srec.specularRay = ray(rec.p, direction, r_in.time());
        return true;
    }



public:
    double ir; // Index of Refraction


private:
    /*
    * 现实世界中的玻璃, 发生折射的概率会随着入射角而改变——从一个很狭窄的角度去看玻璃窗, 它会变成一面镜子
    * Christophe Schlick提出的数学近似式
    */
    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

/*
* 对玻璃球的trick
* 这里有个简单又好用的trick, 如果你将球的半径设为负值, 形状看上去并没什么变化, 但是法相全都翻转到内部去了
* 所以就可以用这个特性来做出一个通透的玻璃球:
* 把一个小球套在大球里, 光线发生两次折射, 于是负负得正, 上下不会颠倒
*/

#endif