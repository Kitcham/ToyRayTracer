#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "ray.h"
#include "vec3.h"
#include "intersect.h"
#include "texture.h"
#include "intersect_list.h"

/*
    class��material ���ʻ���
    function��
    1�� ����ɢ���Ĺ��ߣ�������������ߺ��ɢ�䣩
    2�� ���ɢ�䣬ɢ�����ߵı䰵���attenuate
*/

struct hitRecord;

class material {
public:
    virtual bool scatter (
        //const ray& r_in, const hitRecord& rec, color& attenuation, ray& scattered
        const ray& r_in, const hitRecord& rec, color& albedo, ray& scattered, double& pdf
    ) const 
    {
        return false;
    }

    virtual double scattering_pdf(
        const ray& r_in, const hitRecord& rec, const ray& scattered
    ) const 
    {
        return 0;
    }

    virtual color emitted(const ray& r_in, const hitRecord& rec, double u, double v, const point3& p
    ) const 
    {
        return color(0, 0, 0);
    }
};


#endif