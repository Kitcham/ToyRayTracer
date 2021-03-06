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


#include <iostream>
#include <memory>
#include <vector>
#include "bvh.h"



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

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                    world.add(make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);

                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
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

color ray_color(const ray& r, const color& background, const intersect& world, int depth) {
    hitRecord rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.material_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.material_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
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

int main() {

    // Image

    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int max_depth = 50;
    int samples_per_pixel = 100;

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";


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

    switch (0) {
    case 1:   // 随机球
        world = random_scene();
        background = color(0.70, 0.80, 1.00);
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        aperture = 0.1;
        break;

    case 2:  // 两个纹理球
        world = two_spheres();
        background = color(0.70, 0.80, 1.00);
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        break;

    case 3: // 两个柏林噪声球
        world = two_perlin_spheres();
        background = color(0.70, 0.80, 1.00);
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        break;

    case 4: // 地球贴图
        world = earth();
        background = color(0.70, 0.80, 1.00);
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        break;

    default:
    case 5:  // 矩形光源测试
        world = simple_light();
        samples_per_pixel = 400;
        background = color(0, 0, 0);
        lookfrom = point3(26, 3, 6);
        //lookfrom = point3(26, 0, 0);
        lookat = point3(0, 2, 0);
        vfov = 20.0;
        break;
    }
    // camera参数最终敲定
    auto time_0 = 0.0;
    auto time_1 = 1.0;

    camera camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, time_0, time_1);

    // Render

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
}