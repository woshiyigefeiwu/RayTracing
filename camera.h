//camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera 
{
    public:
        /*
            lookfrom��������ڵĵ�
            lookat���������ĵ�
            vup������Ĵ�ֱ���ϵ�����
            vfov��fov�Ƕ�
            aspect����ߵı���
            aperture����Ȧ�Ŀ׾����������Ϊ͸����ֱ����С
            focus_dist�����࣬����ģ�⾰��Ч��
            t0, t1����������ʱ��η�������
        */
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, double vfov, double aspect, double aperture, double focus_dist)
        {
            auto theta = degrees_to_radians(vfov);
            auto half_height = tan(theta/2);
            auto half_width = aspect * half_height;

            // ��һ�������������������
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

        // ��ȡ����
        ray get_ray(double s, double t) 
        {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();

            return ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset);        
        }

    public:
        vec3 origin;                // ������ʼ��
        vec3 lower_left_corner;     // ���Ͻǵĵ�
        vec3 horizontal;            // ��Ļ�ĳ�
        vec3 vertical;              // ��Ļ�Ŀ�
        vec3 u, v, w;               // �������������������
        double lens_radius;         // ͸���İ뾶��С
};

#endif