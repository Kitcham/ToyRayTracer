#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "rtweekend.h"
#include "ray.h"
#include "vec3.h"
#include "intersect.h"
#include "material.h"
#include "texture.h"

/*
    class��lambertian �ʲ��壨��ȫ�������壩
    inherit��material
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

        // attenuation����˥���ʣ���ÿ��ɢ��󣬹��ߵ�˥�����
        // Ҫô�ǹ�����Զ����ɢ��, ÿ��ɢ��˥����R, Ҫô�ǹ��߲���˥��, ת����������(1-R)�Ĺ���
        // Albedo�����˹��߻��б��������һ��������״̬��������û��ɢ�䣬���߼���ʧ
        // ͬʱ��Ҫ��֤ɢ����ߵ�ʱ����������ߵ�ʱ��һ��
        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<texture> albedo;
};

#endif