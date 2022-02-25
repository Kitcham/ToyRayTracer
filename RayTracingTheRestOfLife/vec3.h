#ifndef VEC3_H
#define VEC3_H

#include "rtweekend.h"


#include <cmath>
#include <iostream>

using std::sqrt;

class vec3 
{
    public:
        vec3() : e{ 0,0,0 } {}
        vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        /* 
        * 反向
        */
        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        
        /*
        * 返回元素
        */
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        /*
        * 加法
        */
        vec3& operator+=(const vec3& v) 
        {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        /*
        * 数乘
        */
        vec3& operator*=(const double t) 
        {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        /*
        * 数字除法（对数字）
        */
        vec3& operator/=(const double t) 
        {
            return *this *= 1 / t;
        }

        /*
        * 求向量长度
        */
        double length() const 
        {
            return sqrt(length_squared());
        }
        /*
        * 向量自平方
        */
        double length_squared() const 
        {
            return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
        }

        /*
        * 对每个像素写入颜色工具函数
        */
        void write_color(std::ostream& out) {

            // Write the translated [0,255] value of each color component.
            out << static_cast<int>(255.999 * e[0]) << ' '
                << static_cast<int>(255.999 * e[1]) << ' '
                << static_cast<int>(255.999 * e[2]) << '\n';
        }
        
        void write_color(std::ostream& out, int samples_per_pixel) {
            // Divide the color total by the number of samples.
            auto scale = 1.0 / samples_per_pixel;
            //auto r = scale * e[0];
            //auto g = scale * e[1];
            //auto b = scale * e[2];
            // gamma 2空间 颜色指数为 1/gamma 即开平方
            auto r = sqrt(scale * e[0]);
            auto g = sqrt(scale * e[1]);
            auto b = sqrt(scale * e[2]);
            
            // Write the translated [0,255] value of each color component.
            out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
                << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
                << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
        }

        /*
        * 判断是否非常接近於0
        */
        bool near_zero() const {
            // Return true if the vector is close to zero in all dimensions.
            const auto s = 1e-8;
            return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
        }

        /*
        * 用于漫反射随机生成，返回一个随机vec3
        */
        inline static vec3 random()
        {
            return vec3(random_double(), random_double(), random_double());
        }
        // 在指定范围内生成
        inline static vec3 random(double min, double max)
        {
            return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
        }

       

    public:
        double e[3];
};

// 别名，以便区分其为点，向量，抑或是颜色值
using point3 = vec3;   // 3D point
using color = vec3;    // RGB color


// vec3 Utility Functions 工具类函数
/*
* 输出某个vec3，用于重载
*/
inline std::ostream& operator<<(std::ostream& out, const vec3& v) 
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}
/*
* 两个vec3的相加
*/
inline vec3 operator+(const vec3& u, const vec3& v) 
{
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}
/*
* 两个vec3的减法
*/
inline vec3 operator-(const vec3& u, const vec3& v) 
{
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}
/*
* 两个vec3的点乘
*/
inline vec3 operator*(const vec3& u, const vec3& v) 
{
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}
/*
* vec3的数乘
*/
inline vec3 operator*(double t, const vec3& v) 
{
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) 
{
    return t * v;
}
/*
* vec3的数除
*/
inline vec3 operator/(vec3 v, double t) 
{
    return (1 / t) * v;
}
/*
* 点乘工具类
*/
inline double dot(const vec3& u, const vec3& v) 
{
    return u.e[0] * v.e[0]
        + u.e[1] * v.e[1]
        + u.e[2] * v.e[2];
}
/*
* 叉乘工具类
*/
inline vec3 cross(const vec3& u, const vec3& v) 
{
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
        u.e[2] * v.e[0] - u.e[0] * v.e[2],
        u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}
/*
* 求单位向量
*/
inline vec3 unit_vector(vec3 v) 
{
    return v / v.length();
}

/*
* 在（-1，1）内生成随机点
* 否定法，lambertian漫反射
*/
vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

/*
* lambertian的分布律的修正
* 拒绝法生成的点是单位球体积内的的随机点, 
* 这样生成的向量大概率上会和法线方向相近, 并且极小概率会沿着入射方向反射回去
* 真正的lambertian散射后的光线距离法相比较近的概率会更高, 但是分布律会更加均衡
* 我们可以通过在单位球内选取一个随机点, 然后将其单位化来获得该点
*/
// 极坐标形式
vec3 random_unit_vector() 
{
    auto a = random_double(0, 2 * pi);
    auto z = random_double(-1, 1);
    auto r = sqrt(1 - z * z);
    return vec3(r * cos(a), r * sin(a), z);
}
/*
* 直接从入射点开始选取一个随机的方向, 然后再判断是否在法向量所在的那个半球
* 在使用lambertian漫发射模型前, 早期的光线追踪论文中大部分使用的都是该方法
*/
vec3 random_in_hemisphere(const vec3& normal) 
{
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

// 反射
vec3 reflect(const vec3& v, const vec3& n) 
{
    return v - 2 * dot(v, n) * n;
}

// 折射
// Snell法则定义：n*Sin0 = n'*Sin0'

vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

// 从一个单位小圆盘射出光线
vec3 random_in_unit_disk() 
{
    while (true) 
    {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0); // 在该点所在xoy平面内，半径为1范围内产生点
        if (p.length_squared() >= 1) // 超半径1则重新产生，否则返回该产生点的坐标
        {
            continue;
        }
        return p;
    }
}



#endif