#include "Bvh.h"
aabb surrounding_box(aabb box0, aabb box1) {
    glm::vec3 small(
        fmin(box0.minimum.x, box1.minimum.x),
        fmin(box0.minimum.y, box1.minimum.y),
        fmin(box0.minimum.z, box1.minimum.z)
    );
    glm::vec3 big(
        fmax(box0.maximum.x, box1.maximum.x),
        fmax(box0.maximum.y, box1.maximum.y),
        fmax(box0.maximum.z, box1.maximum.z)
    );
    return aabb(small, big);
}
aabb Bvh::build(int now, int start, int end)
{
    int axis = random_int(0, 2);
    auto comparator = (axis == 0) ? box_x_compare : (axis == 2) ? box_y_compare : box_z_compare;
    int object_span = end - start;

    int Left, Right;
    aabb box_left, box_right;
    Left = now << 1, Right = (now << 1) + 1;

    if (object_span == 1) {
        BVHList[Left].Tri = BVHList[Right].Tri = TriList[start];
        BVHList[Left].TriFlag = BVHList[Right].TriFlag = 1.0f;
    }
    else if (object_span == 2) {
        if (comparator(TriList[start], TriList[start + 1])) {
            BVHList[Left].Tri = TriList[start];
            BVHList[Right].Tri = TriList[start + 1];
            BVHList[Left].TriFlag = BVHList[Right].TriFlag = 1.0f;
        }
        else {
            BVHList[Left].Tri = TriList[start + 1];
            BVHList[Right].Tri = TriList[start];
            BVHList[Left].TriFlag = BVHList[Right].TriFlag = 1.0f;
        }
    }
    else {
        std::sort(TriList.begin() + start, TriList.begin() + end, comparator);

        int mid = start + object_span / 2;
        BVHList[Left].AABB = build(Left, start, mid);
        BVHList[Left].TriFlag = 2.0f;
        box_left = BVHList[Left].AABB;

        BVHList[Right].AABB = build(Right, mid, end);
        box_right = BVHList[Right].AABB;
        BVHList[Right].TriFlag = 2.0f;
    }

    if (BVHList[Left].TriFlag==1) {
        BVHList[Left].Tri.bounding_box(box_left);
    }
    if (BVHList[Right].TriFlag==1) {
        BVHList[Right].Tri.bounding_box(box_right);
    }

    return surrounding_box(box_left, box_right);
}
