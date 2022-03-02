#ifndef METAL_H
#define METAL_H

#include "rtweekend.h"
#include "ray.h"
#include "vec3.h"
#include "intersect.h"

/*
    class��metal ������
    inherit��material
*/

class metal : public material{
    public:
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}
        /* ʹ��pdfǰ
        virtual bool scatter(
            const ray & r_in, const hitRecord& rec, color & attenuation, ray & scattered
        ) const {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);// С��0Ϊ����
        }
        */
        virtual bool scatter(
            const ray& r_in, const hitRecord& rec, scatterRecord& srec
        ) const override {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            srec.specularRay = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
            srec.attenuation = albedo;
            srec.isSpecular = true;
            srec.pdf_ptr = 0;
            return true;
        }

    public:
        color albedo;
        double fuzz; //ģ���̶ȣ�=0ʱ��ģ����������Խ������Խģ��*
};




#endif