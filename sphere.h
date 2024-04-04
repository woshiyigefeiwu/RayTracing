// sphere.h：球体

#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "rtweekend.h"

// 普通的球

class sphere: public hittable 
{
    public:
        sphere() {}
        sphere(vec3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;

    public:
        vec3 center;
        double radius;
        shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const 
{
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;
    auto discriminant = half_b*half_b - a*c;

    // 相交
    if (discriminant > 0) 
    {
        auto root = sqrt(discriminant);

        // 判断第一个解
        auto temp = (-half_b - root)/a;
        if (temp < t_max && temp > t_min) 
        {
            rec.t = temp;
            rec.p = r.at(rec.t);
            // 加入射入面的判断
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }

        // 判断第二个解
        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) 
        {
            rec.t = temp;
            rec.p = r.at(rec.t);    // 交点
            // 加入射入面的判断
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

#endif



