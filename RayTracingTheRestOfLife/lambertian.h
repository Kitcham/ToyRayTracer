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
    class��lambertian �ʲ��壨��ȫ�������壩
    inherit��material
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
        // ����һ�����Դ���������ߵ�PDF���������ϵ�MC����
        // ��f(x)��f(r)/p(r)
        // ���ʲ��壬p(direction)=cos(��)/��
        //pdf = dot(rec.normal, scattered.direction()) / pi;
        */

        /*
        pdf = 0.5 / pi;   // ����������
        */
        pdf = dot(uvw.w(), scattered.direction()) / pi;
        return true;
        
        /*
        // attenuation����˥���ʣ���ÿ��ɢ��󣬹��ߵ�˥�����
        // Ҫô�ǹ�����Զ����ɢ��, ÿ��ɢ��˥����R, Ҫô�ǹ��߲���˥��, ת����������(1-R)�Ĺ���
        // Albedo�����˹��߻��б��������һ��������״̬��������û��ɢ�䣬���߼���ʧ
        // ͬʱ��Ҫ��֤ɢ����ߵ�ʱ����������ߵ�ʱ��һ��
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