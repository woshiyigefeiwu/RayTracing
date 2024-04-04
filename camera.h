//camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera 
{
    public:
        /*
            lookfrom：相机所在的点
            lookat：相机看向的点
            vup：相机的垂直向上的向量
            vfov：fov角度
            aspect：宽高的比例
            aperture：光圈的孔径，可以理解为透镜的直径大小
            focus_dist：焦距，用于模拟景深效果
            t0, t1：相机在这个时间段发出光线
        */
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, double vfov, double aspect, double aperture, double focus_dist, double t0 = 0, double t1 = 0)
        {
            time0 = t0;
            time1 = t1;

            auto theta = degrees_to_radians(vfov);
            auto half_height = tan(theta/2);
            auto half_width = aspect * half_height;

            // 求一下描述相机的三个向量
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            lens_radius = aperture / 2;
            origin = lookfrom;
            lower_left_corner = origin
                              - half_width * focus_dist * u
                              - half_height * focus_dist * v
                              - focus_dist * w;
            horizontal = 2*half_width*focus_dist*u;
            vertical = 2*half_height*focus_dist*v;
        }

        // 获取光线
        ray get_ray(double s, double t) 
        {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();

            return ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset, random_double(time0, time1));        
        }

    public:
        vec3 origin;                // 光线起始点
        vec3 lower_left_corner;     // 左上角的点
        vec3 horizontal;            // 屏幕的长
        vec3 vertical;              // 屏幕的宽
        vec3 u, v, w;               // 描述摄像机的三个向量
        double lens_radius;         // 透镜的半径大小
        double time0, time1;        // 摄像机在这个时间段内随机发出光线
};

#endif