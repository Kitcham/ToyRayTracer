#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "intersect.h"
#include "vec3.h"

class TriangleMesh : public intersect {
public:
	TriangleMesh(vec3 v1u, vec3 v2u, vec3 v3u, vec3 kdu, vec3 ksu, int v1ii, int v2ii, int v3ii)
		: v1(v1u), v2(v2u), v3(v3u), kd(kdu), ks(ksu), v1i(v1ii), v2i(v2ii), v3i(v3ii) {};

	virtual bool hit(const ray& r, double t_min, double t_max, hitRecord& rec) const;
	virtual bool bounding_box(double time0, double time1, aabb& output_box) const;

private:
	vec3 v1;
	vec3 v2;
	vec3 v3;
	vec3 kd;
	vec3 ks;
	int v1i;
	int v2i;
	int v3i;
};

bool TriangleMesh::hit(const ray& r, double t_min, double t_max, hitRecord& rec) const {
	double epsilon = 1e-5;
	vec3 edge1 = v2 - v1;
	vec3 edge2 = v3 - v1;
	vec3 n = cross(edge1, edge2);
	vec3 h = cross(r.direction(), edge2);
	double a = dot(edge1, h);
	if (a > -epsilon && a < epsilon) {
		// ray is parallel to triangle
		return false;
	}

	double f = 1.0 / a;
	vec3 s = r.origin() - v1;
	double u = f * dot(s, h);
	if (u < 0 || u > 1) {
		return false;
	}

	vec3 q = cross(s, edge1);
	double v = f * dot(r.direction(), q);
	if (v < 0 || u + v > 1) {
		return false;
	}

	double t = f * dot(edge2, q);
	if (t < 0 || t < t_min || t_max < t) {
		return false;
	}

	if (t > epsilon) {
		rec.t = t;
		rec.p = r.at(rec.t);
		rec.normal = unit_vector(n);
		rec.kd = kd;
		rec.ks = ks;
		rec.v1i = v1i;
		rec.v2i = v2i;
		rec.v3i = v3i;
		return true;
	}

	return false;
}


bool TriangleMesh::bounding_box(double time0, double time1, aabb& output_box) const {
	double minx = fmin(v1[0], fmin(v2[0], v3[0]));
	double miny = fmin(v1[1], fmin(v2[1], v3[1]));
	double minz = fmin(v1[2], fmin(v2[2], v3[2]));

	double maxx = fmax(v1[0], fmax(v2[0], v3[0]));
	double maxy = fmax(v1[1], fmax(v2[1], v3[1]));
	double maxz = fmax(v1[2], fmax(v2[2], v3[2]));
	double eps = 1e-5;

	vec3 min = vec3(minx - eps, miny - eps, minz - eps);
	vec3 max = vec3(maxx + eps, maxy + eps, maxz + eps);
	//std::cout << "min = " << min << std::endl;
	//std::cout << "max = " << max << std::endl;
	output_box = aabb(min, max);
	return true;
}

#endif