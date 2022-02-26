#ifndef DIFFUSE_LIGHT_H
#define DIFFUSE_LIGHT_H

#include "rtweekend.h"
#include "ray.h"
#include "vec3.h"
#include "intersect.h"
#include "material.h"

/*
    Class:diffuse_light �������
    inherit��material
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
    // �컨���ϵĵ���Χ�����������ڵ���˫���
    // �ƺ��컨��֮����һ����С�Ŀռ䡣
    // Ҫ�ý������·��䣬����ͨ����intersect�����ĳ�Ա������ȡ�������Ϣ������
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