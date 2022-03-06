/*
* 对可求交物体的方向进行采样，如光源等
*/

#include "intersect.h"

class intersect_pdf : public pdf {
public:
    intersect_pdf(shared_ptr<intersect> p, const point3& origin) : ptr(p), o(origin) {}

    virtual double value(const vec3& direction) const override {
        return ptr->pdf_value(o, direction);
    }

    virtual vec3 generate() const override {
        return ptr->random(o);
    }

public:
    point3 o;
    shared_ptr<intersect> ptr;
};