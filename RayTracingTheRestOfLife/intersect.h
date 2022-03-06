#ifndef INTERSECT_H
#define INTERSECT_H

#include "ray.h"
#include "rtweekend.h"
#include "material.h"
#include "aabb.h"
#include "onb.h"

/*
	Class:intersect 记录相交容器
*/

class material;

struct hitRecord
{
	double t;        //ray 中的系数t
	bool frontFace;    // 视线射入面，true
	vec3 p;            //相交点、撞击点
	vec3 normal;            //_p点的表面法线
	double u;
	double v;
	shared_ptr<material> material_ptr;

	vec3 kd;  // ambient (Ka), diffuse (Kd), specular (Ks)
	vec3 ks;  
	int v1i;
	int v2i;
	int v3i;
	

	inline void set_face_normal(const ray& sight, const vec3& outwardNormal)
	{
		frontFace = dot(sight.direction(), outwardNormal) < 0;
		normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

class intersect
{
	public:
		intersect() {  }
		
		constexpr static double inf() { return 0x3f3f3f3f; }        //最大值

		   /*
			@brief: 撞击函数，求取撞击点相关记录信息
			@param: sight->视线
			系数t的上下界->筛选撞击点
			rec->返回撞击点信息
			@retur: 是否存在合法撞击点
			*/
		virtual bool hit(const ray& sight, double tMin, double tMax, hitRecord& rec)const = 0;
	
		virtual ~intersect() {  }

		virtual bool bounding_box(double time0, double time1, aabb& output_box) const = 0;

		// 避免对所有intersect子类进行pdf检测，使用虚函数
		virtual double pdf_value(const point3& o, const vec3& v) const 
		{
			return 0.0;
		}

		virtual vec3 random(const vec3& o) const 
		{
			return vec3(1, 0, 0);
		}
		
};




class translate : public intersect {
public:
	translate(shared_ptr<intersect> p, const vec3& displacement)
		: ptr(p), offset(displacement) {}

	virtual bool hit(
		const ray& r, double t_min, double t_max, hitRecord& rec) const override;

	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

public:
	shared_ptr<intersect> ptr;
	vec3 offset;
};

bool translate::hit(const ray& r, double t_min, double t_max, hitRecord& rec) const {
	ray moved_r(r.origin() - offset, r.direction(), r.time());
	if (!ptr->hit(moved_r, t_min, t_max, rec))
		return false;

	rec.p += offset;
	rec.set_face_normal(moved_r, rec.normal);

	return true;
}

bool translate::bounding_box(double time0, double time1, aabb& output_box) const {
	if (!ptr->bounding_box(time0, time1, output_box))
		return false;

	output_box = aabb(
		output_box.min() + offset,
		output_box.max() + offset);

	return true;
}




class rotate_y : public intersect {
public:
	rotate_y(shared_ptr<intersect> p, double angle);

	virtual bool hit(
		const ray& r, double t_min, double t_max, hitRecord& rec) const override;

	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
		output_box = bbox;
		return hasbox;
	}

public:
	shared_ptr<intersect> ptr;
	double sin_theta;
	double cos_theta;
	bool hasbox;
	aabb bbox;
};

rotate_y::rotate_y(shared_ptr<intersect> p, double angle) : ptr(p) {
	auto radians = degrees_to_radians(angle);
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	hasbox = ptr->bounding_box(0, 1, bbox);

	point3 min(infinity, infinity, infinity);
	point3 max(-infinity, -infinity, -infinity);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
				auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
				auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

				auto newx = cos_theta * x + sin_theta * z;
				auto newz = -sin_theta * x + cos_theta * z;

				vec3 tester(newx, y, newz);

				for (int c = 0; c < 3; c++) {
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}
			}
		}
	}

	bbox = aabb(min, max);
}

bool rotate_y::hit(const ray& r, double t_min, double t_max, hitRecord& rec) const {
	auto origin = r.origin();
	auto direction = r.direction();

	origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
	origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

	direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
	direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

	ray rotated_r(origin, direction, r.time());

	if (!ptr->hit(rotated_r, t_min, t_max, rec))
		return false;

	auto p = rec.p;
	auto normal = rec.normal;

	p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
	p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

	normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
	normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

	rec.p = p;
	rec.set_face_normal(rotated_r, normal);

	return true;
}

// 翻转灯光，使其法线指向 -y 方向:
class flip_face : public intersect {
public:
	flip_face(shared_ptr<intersect> p) : ptr(p) {}

	virtual bool hit(
		const ray& r, double t_min, double t_max, hitRecord& rec) const override {

		if (!ptr->hit(r, t_min, t_max, rec))
			return false;

		rec.frontFace = !rec.frontFace;
		return true;
	}

	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
		return ptr->bounding_box(time0, time1, output_box);
	}

public:
	shared_ptr<intersect> ptr;
};

#endif    //INTERSECT_H