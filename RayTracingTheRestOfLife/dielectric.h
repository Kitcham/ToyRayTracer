#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "rtweekend.h"
#include "ray.h"
#include "vec3.h"
#include "intersect.h"
#include "material.h"

/*
    Class:dielectric ��Ե��
    inherit��material
*/

class dielectric : public material {
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}
    /*ʹ��pdfǰ
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

        // ��������ʱ��Sniff�����޽⣬��Ϊ�����˷���
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

        // ��������ʱ��Sniff�����޽⣬��Ϊ�����˷���
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
    * ��ʵ�����еĲ���, ��������ĸ��ʻ���������Ƕ��ı䡪����һ������խ�ĽǶ�ȥ��������, ������һ�澵��
    * Christophe Schlick�������ѧ����ʽ
    */
    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

/*
* �Բ������trick
* �����и����ֺ��õ�trick, ����㽫��İ뾶��Ϊ��ֵ, ��״����ȥ��ûʲô�仯, ���Ƿ���ȫ����ת���ڲ�ȥ��
* ���ԾͿ������������������һ��ͨ͸�Ĳ�����:
* ��һ��С�����ڴ�����, ���߷�����������, ���Ǹ�������, ���²���ߵ�
*/

#endif