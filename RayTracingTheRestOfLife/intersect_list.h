#ifndef INTERSECT_LIST_H
#define INTERSECT_LIST_H

#include "intersect.h"
#include "aabb.h"
#include <memory>
#include <vector>

/*
    Class:intersectList 相交容器表
*/

using std::shared_ptr;
using std::make_shared;

class intersectList : public intersect {
public:
    intersectList() {}
    intersectList(shared_ptr<intersect> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<intersect> object) { objects.push_back(object); }

    virtual bool hit(const ray& r, double tMin, double tMax, hitRecord& rec) const override;

    virtual bool bounding_box(
        double time0, double time1, aabb& output_box) const override;

public:
    std::vector<shared_ptr<intersect>> objects;
};

bool intersectList::hit(const ray& r, double tMin, double tMax, hitRecord& rec) const 
{
    hitRecord tempRec;
    bool hitAnything = false;
    auto closestSoFar = tMax;  //刚开始可以看到无限远

    for (const auto& object : objects) 
    {
        if (object->hit(r, tMin, closestSoFar, tempRec)) 
        {
            hitAnything = true;
            closestSoFar = tempRec.t;    //将上一次的最近撞击点作为视线可达最远处
            rec = tempRec;
        }
    }

    return hitAnything;
}

bool intersectList::bounding_box(double time0, double time1, aabb& output_box) const {
    if (objects.empty()) return false;

    aabb temp_box;
    bool first_box = true;

    for (const auto& object : objects) {
        if (!object->bounding_box(time0, time1, temp_box)) return false;
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }

    return true;
}

#endif