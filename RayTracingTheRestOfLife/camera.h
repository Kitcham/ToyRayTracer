#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"
#include "vec3.h"
#include "ray.h"

/*
    Class:camera 相机
*/

class camera {
public:
    
    camera(
        point3 lookfrom,  // 视线出发点，相机所在位置
        point3 lookat,     // 视线观察的点
        vec3   vup,       // 屏幕上边界
        double vfov, // 即相机在垂直方向上从屏幕顶端扫描到底部所岔开的视角角度
        double aspect_ratio, // 宽高比
        double aperture,     // 光圈
        double focus_dist,    // 焦距
        double _time0 = 0,
        double _time1 = 0
    ) 
    {

        /*
            vfov即相机在垂直方向上从屏幕顶端扫描到底部所岔开的视角角度
            hfov即相机在水平方向上从屏幕左端扫描到右端所岔开的视角角度
            眼睛离屏幕中心的距离为dis(也就是1)

            根据 tan(vfov/2) = (屏幕高/2)/dis
            得到 屏幕高 = 2 * dis * tan(vfov/2)
            则，屏幕上边界为 y_up = dis * tan(vfov/2)
            　　屏幕下边界为 y_bottom = - y_up　
            屏幕宽 = 屏幕高 * aspect
            则，屏幕左边界为 x_left = - 屏幕宽/2
　　            屏幕右边界为 x_right = - x_left
        */
        auto theta = degrees_to_radians(vfov);  // 由屏幕顶端扫描到屏幕中点位置的角度的张角
        auto half_viewport_height = tan(theta / 2); // 屏幕半高
        auto viewport_height = 2.0 * half_viewport_height;       // 屏幕高
        auto viewport_width = aspect_ratio * viewport_height;    // 屏幕宽


        /*
            因为相机坐标系算是一个局部性质的，当我们把它放在世界坐标系中，
            就需要用一个始终指向世界坐标系正上方的基向量vup(view up)
            w是一个正交于相机平面的基向量,与视线反向的一个基向量
            有了w，我们便可以定义u了，u向量代表相机平面的水平向量
            u一定平行于世界坐标系的x轴
            vup平行于世界坐标系的y轴
            所以u⊥vup
            而w⊥相机平面，所以w⊥u
            所以  u = vup × w   该体系为右手系
            最后定义v，v就是相机平面的垂直方向
            即， v = w × u
        */
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        /*
            先前为便利假定成像平面位于z = -1(或者是-w平面,按w基向量算)
            所以 tan(theta/2) = (h/2) / dis ,其中dis为1
            而改进后，应该使焦距dis真正变成一个变量，即：焦距（镜片到成像平面之间的距离）
            随之，成像平面也就到了z = -focus，或者是-focus * w平面(按w基向量算)
            所有光线都从内部的一个虚拟透镜发出, 经过lookfrom点, 这个透镜的半径越大, 图像就越模糊
        */
        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

        lens_radius = aperture / 2;

        time0 = _time0;
        time1 = _time1;
    }

    ray get_ray(double s, double t) const 
    {
        vec3 rd = lens_radius * random_in_unit_disk(); // 随机坐标
        vec3 offset = u * rd.x() + v * rd.y();    // 将随即量放到相机平面中，计算光线发生位置

        return ray(
            origin + offset, // 光线原点
            lower_left_corner + s * horizontal + t * vertical - origin - offset, // 方向
            random_double(time0, time1)  
        );
    }

private:
    point3 origin;               // 视线原点
    point3 lower_left_corner;    // 左下角坐标
    vec3 horizontal;            
    vec3 vertical;
    vec3 u, v, w;
    double lens_radius;         // 焦距
    double time0, time1;  // shutter open/close times
};

/*
class camera {
    public:
        camera() {
            lower_left_corner = vec3(-2.0, -1.0, -1.0);
            horizontal = vec3(4.0, 0.0, 0.0);
            vertical = vec3(0.0, 2.0, 0.0);
            origin = vec3(0.0, 0.0, 0.0);
        }

        ray get_ray(double u, double v) {
            return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
        }

    public:
        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
};
*/
#endif