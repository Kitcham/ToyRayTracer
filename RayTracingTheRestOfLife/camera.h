#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"
#include "vec3.h"
#include "ray.h"

/*
    Class:camera ���
*/

class camera {
public:
    
    camera(
        point3 lookfrom,  // ���߳����㣬�������λ��
        point3 lookat,     // ���߹۲�ĵ�
        vec3   vup,       // ��Ļ�ϱ߽�
        double vfov, // ������ڴ�ֱ�����ϴ���Ļ����ɨ�赽�ײ��������ӽǽǶ�
        double aspect_ratio, // ��߱�
        double aperture,     // ��Ȧ
        double focus_dist,    // ����
        double _time0 = 0,
        double _time1 = 0
    ) 
    {

        /*
            vfov������ڴ�ֱ�����ϴ���Ļ����ɨ�赽�ײ��������ӽǽǶ�
            hfov�������ˮƽ�����ϴ���Ļ���ɨ�赽�Ҷ��������ӽǽǶ�
            �۾�����Ļ���ĵľ���Ϊdis(Ҳ����1)

            ���� tan(vfov/2) = (��Ļ��/2)/dis
            �õ� ��Ļ�� = 2 * dis * tan(vfov/2)
            ����Ļ�ϱ߽�Ϊ y_up = dis * tan(vfov/2)
            ������Ļ�±߽�Ϊ y_bottom = - y_up��
            ��Ļ�� = ��Ļ�� * aspect
            ����Ļ��߽�Ϊ x_left = - ��Ļ��/2
����            ��Ļ�ұ߽�Ϊ x_right = - x_left
        */
        auto theta = degrees_to_radians(vfov);  // ����Ļ����ɨ�赽��Ļ�е�λ�õĽǶȵ��Ž�
        auto half_viewport_height = tan(theta / 2); // ��Ļ���
        auto viewport_height = 2.0 * half_viewport_height;       // ��Ļ��
        auto viewport_width = aspect_ratio * viewport_height;    // ��Ļ��


        /*
            ��Ϊ�������ϵ����һ���ֲ����ʵģ������ǰ���������������ϵ�У�
            ����Ҫ��һ��ʼ��ָ����������ϵ���Ϸ��Ļ�����vup(view up)
            w��һ�����������ƽ��Ļ�����,�����߷����һ��������
            ����w�����Ǳ���Զ���u�ˣ�u�����������ƽ���ˮƽ����
            uһ��ƽ������������ϵ��x��
            vupƽ������������ϵ��y��
            ����u��vup
            ��w�����ƽ�棬����w��u
            ����  u = vup �� w   ����ϵΪ����ϵ
            �����v��v�������ƽ��Ĵ�ֱ����
            ���� v = w �� u
        */
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        /*
            ��ǰΪ�����ٶ�����ƽ��λ��z = -1(������-wƽ��,��w��������)
            ���� tan(theta/2) = (h/2) / dis ,����disΪ1
            ���Ľ���Ӧ��ʹ����dis�������һ���������������ࣨ��Ƭ������ƽ��֮��ľ��룩
            ��֮������ƽ��Ҳ�͵���z = -focus��������-focus * wƽ��(��w��������)
            ���й��߶����ڲ���һ������͸������, ����lookfrom��, ���͸���İ뾶Խ��, ͼ���Խģ��
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
        vec3 rd = lens_radius * random_in_unit_disk(); // �������
        vec3 offset = u * rd.x() + v * rd.y();    // ���漴���ŵ����ƽ���У�������߷���λ��

        return ray(
            origin + offset, // ����ԭ��
            lower_left_corner + s * horizontal + t * vertical - origin - offset, // ����
            random_double(time0, time1)  
        );
    }

private:
    point3 origin;               // ����ԭ��
    point3 lower_left_corner;    // ���½�����
    vec3 horizontal;            
    vec3 vertical;
    vec3 u, v, w;
    double lens_radius;         // ����
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