#ifndef SPHERE_H
#define SPHERE_H

#include "intersect.h"
#include "material.h"
#include "vec3.h"
#include "rtweekend.h"

/*
    class：sphere 球体
*/

class sphere : public intersect {
    public:
        sphere() {}
        // @param: centerPos->球心坐标，radius->半径
        sphere(vec3 centerPos, double radius) : center(centerPos), radius(radius) {};

        sphere(vec3 centerPos, double radius, shared_ptr<material> material)
            : center(centerPos), radius(radius), material_ptr(material) {};
                /*
        @brief: 撞击函数，求取撞击点相关记录信息
        @param: sight->视线
                系数t的上下界->筛选撞击点
                rec->返回撞击点信息
        @retur: 是否存在合法撞击点
        */
        virtual bool hit(const ray& sight, double tMin, double tMax, hitRecord& rec) const;

        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

      


    public:
        vec3 center;                         // 球心
        double radius;                       // 半径
        shared_ptr<material> material_ptr;   // 材质

        /*
            @ get-functions
        
        inline const double getRaidus()const { return radius; }

        inline const vec3& getCenter()const { return center; }

        inline double& getRaidus() { return radius; }

        inline vec3& getCenter() { return center; }*/

    private:
        static void get_sphere_uv(const point3& p, double& u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

            auto theta = acos(-p.y());
            auto phi = atan2(-p.z(), p.x()) + pi;
    
            u = phi / (2 * pi);
            v = theta / pi;
    }
};

bool sphere::hit(const ray& sight, double tMin, double tMax, hitRecord& rec) const 
{
    
    vec3 trace = sight.origin() - center;
    auto a = sight.direction().length_squared();
    //auto half_b = dot(trace, sight.direction());
    auto b = 2.0 * dot(trace, sight.direction());
    auto c = trace.length_squared() - radius * radius;
    auto discriminant = b * b - 4 * a * c;
    //auto discriminant = half_b * half_b - a * c;

    
    if (discriminant > 0) 
    {
        // 获取离视线最近的根
        auto deltaRoot = sqrt(discriminant);
        auto temp = (-b - deltaRoot) / (2.0 * a);
        if (temp < tMax && temp > tMin) 
        {
            rec.t = temp;
            rec.p = sight.at(rec.t);
            vec3 outwardNormal = (rec.p - center) / radius;
            rec.set_face_normal(sight, outwardNormal);
            get_sphere_uv(outwardNormal, rec.u, rec.v);
            rec.material_ptr = material_ptr;
            return true;
        }
        temp = (-b + deltaRoot) / (2.0 * a);
        if (temp < tMax && temp > tMin) 
        {
            rec.t = temp;
            rec.p = sight.at(rec.t);
            vec3 outwardNormal = (rec.p - center) / radius;
            rec.set_face_normal(sight, outwardNormal);
            get_sphere_uv(outwardNormal, rec.u, rec.v);
            rec.material_ptr = material_ptr;
            return true;
        }
    }
    /*
    if (discriminant > 0) {
        auto root = sqrt(discriminant);
        auto temp = (-half_b - root) / a;
        if (temp < tMax && temp > tMin) {
            rec.t = temp;
            rec.p = sight.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(sight, outward_normal);
            return true;
        }
        temp = (-half_b + root) / a;
        if (temp < tMax && temp > tMin) {
            rec.t = temp;
            rec.p = sight.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(sight, outward_normal);
            return true;
        }
    }*/
    return false;
    //return false;
}


bool sphere::bounding_box(double time0, double time1, aabb& output_box) const {
    output_box = aabb(
        center - vec3(radius, radius, radius),
        center + vec3(radius, radius, radius));
    return true;
}


#endif