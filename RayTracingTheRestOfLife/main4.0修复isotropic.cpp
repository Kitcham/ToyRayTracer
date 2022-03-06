#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "rtweekend.h"
#include "intersect_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "lambertian.h"
#include "metal.h"
#include "dielectric.h"
#include "camera.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"
#include "isotropic.h"
#include "diffuse_light.h"
#include "PDF.h"

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include "bvh.h"

#include <limits>
#include <thread>
#include "intersect_pdf.h"




intersectList earth() {
    auto earth_texture = make_shared<image_texture>("C:\\Users\\Kitchun\\source\\repos\\RayTracingInNextWeek\\RayTracingInNextWeek\\testimg\\earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    return intersectList(globe);
}

intersectList two_spheres() {
    intersectList objects;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}

intersectList two_perlin_spheres() {
    intersectList objects;

    auto pertext = make_shared<noise_texture>(4);

    objects.add(make_shared<sphere>(vec3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(vec3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    return objects;
}

intersectList random_scene() {
    intersectList world;

    /*不带纹理
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));
    */

    /*带纹理*/
    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    /*
    * 运动的计算，球由C位置线性运动到 C + (0, r/2, 0), r∈[0,1)

    */

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9)
            {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8)
                {
                    // 漫反射
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                    world.add(make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95)
                {
                    // 金属球
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);

                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else
                {
                    // 玻璃球
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return static_cast<intersectList>(make_shared<bvh_node>(world, 0, 1));
}

intersectList simple_light() {
    intersectList objects;

    auto pertext = make_shared<noise_texture>(4);
    // 一大球一小球
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));


    auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
    // 球光源
    objects.add(make_shared<sphere>(point3(-10, 7, 6), 2, difflight));
    // 矩形光源
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));
    //objects.add(make_shared<xy_rect>(3, 5, 0, 3, 3, difflight));

    return objects;
}

/*PDF调整前
intersectList cornell_box() {
    intersectList objects;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    //objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));  // 未设置单向的灯光
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<intersect> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    objects.add(box1);

    shared_ptr<intersect> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    objects.add(box2);

    return objects;
}*/

intersectList cornell_box() {
    intersectList objects;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    //objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));  // 未设置单向的灯光
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<intersect> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), aluminum);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    objects.add(box1);

    auto glass = make_shared<dielectric>(1.5);
    objects.add(make_shared<sphere>(point3(190, 90, 190), 90, glass));

    return objects;
}

/*PDF修复前
intersectList cornell_smoke() {
    intersectList objects;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    //objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<intersect> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<intersect> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    objects.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
    objects.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));

    return objects;
}
*/

intersectList cornell_smoke() {
    intersectList objects;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    //objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<intersect> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<intersect> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    objects.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
    objects.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));

    auto glass = make_shared<dielectric>(1.5);
    objects.add(make_shared<sphere>(point3(420, 90, 190), 90, glass));

    return objects;
}




intersectList final_scene() {
    intersectList boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    intersectList objects;

    objects.add(make_shared<bvh_node>(boxes1, 0, 1));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
        ));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("C:\\Users\\Kitchun\\source\\repos\\RayTracingInNextWeek\\RayTracingInNextWeek\\testimg\\earthmap.jpg"));
    objects.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

    intersectList boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
        vec3(-100, 270, 395)
        )
    );

    return objects;
}


color ray_color(const ray& r, const color& background, const intersect& world, shared_ptr<intersect>& lights, int depth)
{
    hitRecord rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    // 视线未碰到任何物体，返回背景色
    // 设最小值0.001是为了避免自相交问题，避免物体反射的光线再次打中自己
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    scatterRecord srec;
    color emitted = rec.material_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

    if (!rec.material_ptr->scatter(r, rec, srec))
        return emitted;

    if (srec.isSpecular) {
        return srec.attenuation
            * ray_color(srec.specularRay, background, world, lights, depth - 1);
    }

    auto light_ptr = make_shared<intersect_pdf>(lights, rec.p);
    mixture_pdf p(light_ptr, srec.pdf_ptr);

    ray scattered = ray(rec.p, p.generate(), r.time());
    auto pdf_val = p.value(scattered.direction());

    return emitted
        + srec.attenuation * rec.material_ptr->scattering_pdf(r, rec, scattered)
        * ray_color(scattered, background, world, lights, depth - 1)
        / pdf_val;
}



/*
vec3 ray_color(const ray& sight, const intersect& world, int depth)
{
    hitRecord rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return vec3(0, 0, 0);

    // 此处若设最小值为0，可能导致物体发出光线与自身相交，因为其发出光线与自己距离为0，因此改为0.001
    if (world.hit(sight, 0.001, infinity, rec))
    {
        // vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        // vec3 target = rec.p + rec.normal + random_unit_vector();
        ray scattered;
        color attenuation;
        if (rec.material_ptr->scatter(sight, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(sight.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}
*/

void ThreadMainLoop(std::vector<vec3>& buffer, const intersect& world, const int Width, const int Height,
    const int Samples, const int max_depth, const int numThreads, const int threadId, camera camera, vec3 background, shared_ptr<intersect> lights)
{
    // Random random(threadId + 1);

    for (int j = Height - threadId - 1; j >= 0; j -= numThreads)
    {
        std::cerr << "ThreadId " << threadId << " remains scanlines " << j << std::endl;

        for (int i = 0; i < Width; ++i)
        {
            vec3 pixel_color(0, 0, 0);

            for (int s = 0; s < Samples; ++s)
            {
                const float u = float(i + random_double()) / float(Width);
                const float v = float(j + random_double()) / float(Height);
                ray sight = camera.get_ray(u, v);
                //pixel_color = pixel_color + ray_color(sight, background, world, max_depth);
                color rayColorTemp = ray_color(sight, background, world, lights, max_depth);
                pixel_color = pixel_color + rayColorTemp;
                //pixel_color = pixel_color + ray_color(sight, background, world, lights, max_depth);
            }

            //buffer[j * Width + i] = Clamp(sqrt(color / float(Samples)) * 255.99f, vec3(0.0f), vec3(255.0f));
            buffer[j * Width + i] = pixel_color;
            // float(Samples)) * 255.99f, vec3(0.0f), vec3(255.0f));
        }
    }
}

void MultiThreadedRayTracing(std::vector<vec3>& buffer, const intersect& world, const int Width, const int Height,
    const int Samples, const int max_depth, camera camera, vec3 background, shared_ptr<intersect> lights)
{
    const int numThreads = std::max(1, static_cast<int>(std::thread::hardware_concurrency()) - 2);
    //const int numThreads = 1;
    std::vector<std::thread> threads;

    std::cerr << "Ray-tracing using " << numThreads << " threads" << std::endl;

    for (int t = 0; t != numThreads; ++t)
    {
        threads.emplace_back([=, &buffer, &world]()
            {
                ThreadMainLoop(buffer, world, Width, Height, Samples, max_depth, numThreads, t, camera, background, lights);
            });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }
}

void OutputFramebuffer(const std::vector<vec3>& buffer, const int Width, const int Height, const int samples_per_pixel)
{
    auto scale = 1.0 / samples_per_pixel;
    auto r = 0.0;
    auto g = 0.0;
    auto b = 0.0;

    std::cout << "P3\n" << Width << " " << Height << "\n255\n";

    for (int j = Height; j--;)
    {
        for (int i = 0; i < Width; ++i)
        {
            // 利用NaN不等于自己的特点，尝试将NaN与自己比较
            // 去除在pixel_color = average(samples)因溢出或运算问题导致的NaN黑点
            if (r != r) r = 0.0;
            if (g != g) g = 0.0;
            if (b != b) b = 0.0;

            const vec3& color = buffer[j * Width + i];
            r = sqrt(scale * color.x());
            g = sqrt(scale * color.y());
            b = sqrt(scale * color.z());
            std::cout << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << " "
                << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << " "
                << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << "\n";
        }
    }
}


void Application(int Width, int Height, int samples_per_pixel, intersectList World,
    const int max_depth, camera camera, vec3 background, shared_ptr<intersect> lights)
{

    std::vector<vec3> buffer(Width * Height);

    MultiThreadedRayTracing(buffer, World, Width, Height, samples_per_pixel, max_depth, camera, background, lights);
    OutputFramebuffer(buffer, Width, Height, samples_per_pixel);
}

int main() {

    // Image

    auto aspect_ratio = 16.0 / 9.0;
    int image_width;
    int image_height;
    const int max_depth = 50;
    int samples_per_pixel = 100;




    // Camera 参数预设定

    /*
    vec3 lookfrom(13, 2, 3);
    vec3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    */
    point3 lookfrom;
    point3 lookat;
    point3 vup(0, 1, 0);
    auto vfov = 40.0;
    auto aperture = 0.0;
    auto dist_to_focus = 10.0;
    color background(0, 0, 0);



    // World
    //   亦涉及到一些camera参数的变更
    intersectList world;

    switch (7) {
    case 1:   // 随机球
        world = random_scene();
        image_width = 1200;
        background = color(0.70, 0.80, 1.00);
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        aperture = 0.1;
        break;

    case 2:  // 两个纹理球
        world = two_spheres();
        image_width = 400;
        background = color(0.70, 0.80, 1.00);
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        break;

    case 3: // 两个柏林噪声球
        world = two_perlin_spheres();
        image_width = 400;
        background = color(0.70, 0.80, 1.00);
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        break;

    case 4: // 地球贴图
        world = earth();
        image_width = 400;
        background = color(0.70, 0.80, 1.00);
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        break;

    case 5:  // 矩形光源测试
        world = simple_light();
        image_width = 400;
        samples_per_pixel = 400;
        background = color(0, 0, 0);
        lookfrom = point3(26, 3, 6);
        //lookfrom = point3(26, 0, 0);
        lookat = point3(0, 2, 0);
        vfov = 20.0;
        break;

    case 6: // cornell_box测试
        world = cornell_box();
        aspect_ratio = 1.0;
        image_width = 600;
        samples_per_pixel = 1000; //200
        background = color(0, 0, 0);
        lookfrom = point3(278, 278, -800);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;

    case 7: // cornell_smoke带烟雾测试
        world = cornell_smoke();
        aspect_ratio = 1.0;
        image_width = 60;
        samples_per_pixel = 100; //200
        lookfrom = point3(278, 278, -800);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;
    default:
    case 8: // 最终场景
        world = final_scene();
        aspect_ratio = 1.0; //16.9
        image_width = 800; //800
        samples_per_pixel = 100; //10000
        background = color(0, 0, 0);
        lookfrom = point3(478, 278, -600);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;
    }


    auto lights = make_shared<intersectList>();

    /*
    lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
    lights->add(make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<material>()));
    */

    /*
    intersectList lights;
    lights.add(make_shared<xz_rect>(213, 343, 227, 332, 554, 0));
    lights.add(make_shared<sphere>(point3(190, 90, 190), 90, 0));
    */

    //lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
    //lights->add(make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<material>()));
    
    // 烟雾盒
    lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));

    // camera参数最终敲定
    auto time_0 = 0.0;
    auto time_1 = 1.0;

    camera camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, time_0, time_1);

    // Render

    image_height = static_cast<int>(image_width / aspect_ratio);
    /*
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            vec3 pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s)
            {
                // 随机采样
                auto u = (i + random_double()) / image_width;
                auto v = (j + random_double()) / image_height;
                ray sight = camera.get_ray(u, v);
                pixel_color = pixel_color + ray_color(sight, background, world, max_depth);

            }
            pixel_color.write_color(std::cout, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
    */
    try
    {
        Application(image_width, image_height, samples_per_pixel, world,
            max_depth, camera, background, lights);
        std::cerr << "\nDone.\n";

        return EXIT_SUCCESS;
    }

    catch (const std::exception& exception)
    {
        std::cerr << "ERROR: " << exception.what() << std::endl;
    }

    catch (...)
    {
        std::cerr << "ERROR: unhandled exception" << std::endl;
    }

    return EXIT_FAILURE;

}