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
float aabbSurfaceArea(aabb AABB) {
    float a, b, c;
    a = AABB.maximum.x - AABB.minimum.x;
    b = AABB.maximum.y - AABB.minimum.y;
    c = AABB.maximum.z - AABB.minimum.z;
    return (a * b + a * c + b * c) * 2;
}
int Bvh::SAH(int now, int start, int end) {
    aabb TriAABB,surfaceAABB;
    TriList[start].bounding_box(surfaceAABB);
    for (auto i = start+1; i < end; i++) {
        TriList[i].bounding_box(TriAABB);
        surfaceAABB = surrounding_box(TriAABB, surfaceAABB);
    }
    float Sn = aabbSurfaceArea(surfaceAABB);//计算当前三角形集合的aabb体积，相当于光线命中的概率
    int B = 10;//桶分割次数
    int minCostCoor = 0, mincostIndex = 0; //最优分割方案，轴和B的值
    float minCost = std::numeric_limits<float>::infinity(); //最小花费
    for (int i = 0; i < 3; i++) {
        switch (i) {
        case 0:
            sort(TriList.begin() + start, TriList.begin() + end, box_x_compare);
            break;
        case 1:
            sort(TriList.begin() + start, TriList.begin() + end, box_y_compare);
            break;
        case 2:
            sort(TriList.begin() + start, TriList.begin() + end, box_z_compare);
            break;
        }

        for (int j = 1; j < B; j++) {
            auto miding = start+ ((end-start)*j / B);
            if (miding - start == 0 || end - miding == 0) continue;
            aabb leftAABB, rightAABB;
            leftAABB.maximum = rightAABB.maximum = glm::vec3(-1e8, -1e8, -1e8);
            leftAABB.minimum = leftAABB.minimum = glm::vec3(1e8, 1e8, 1e8);
            for (int k = start ; k < miding && k < TriList.size(); k++) {
                TriList[k].bounding_box(TriAABB);
                leftAABB = surrounding_box(TriAABB, leftAABB);
            }
            for (int k = miding ; k < end && k < TriList.size(); k++) {
                TriList[k].bounding_box(TriAABB);
                rightAABB = surrounding_box(TriAABB, rightAABB);
            }
            float leftArea = aabbSurfaceArea(leftAABB);
            float rightArea = aabbSurfaceArea(rightAABB);
            float cost = 1 + ((miding - start) * rightArea + (end - miding + 1) * leftArea) / Sn;
            if (cost < minCost) {
                minCost = cost;
                mincostIndex = miding;
                minCostCoor = i;
            }
        }
    }
    return mincostIndex;
}