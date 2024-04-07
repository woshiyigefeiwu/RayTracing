// sphere.h：球体

#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "rtweekend.h"

// 普通的球

void get_sphere_uv(const vec3& p, double& u, double& v) 
{
    auto phi = atan2(p.z(), p.x());
    auto theta = asin(p.y());
    u = 1-(phi + pi) / (2*pi);
    v = (theta + pi/2) / pi;
}

class sphere: public hittable 
{
    public:
        sphere() {}
        sphere(vec3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;

        virtual bool bounding_box(double t0, double t1, aabb& output_box) const;

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
            get_sphere_uv((rec.p-center)/radius, rec.u, rec.v);
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

// 求包围盒
bool sphere::bounding_box(double t0, double t1, aabb& output_box) const 
{
    output_box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
    return true;
}

// -------------------------------------------------------------------------------------------------------

// 运动中的球
// 让它的球心在`time0`到`time1`的时间段内从`center0`线性运动到`center1`。超出这个时间段, 这个球心依然在动

class moving_sphere : public hittable 
{
    public:
        moving_sphere() {}
        moving_sphere(vec3 cen0, vec3 cen1, double t0, double t1, double r, shared_ptr<material> m)
            : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m)
        {};

        virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;

        virtual bool bounding_box(double t0, double t1, aabb& output_box) const;

        vec3 center(double time) const;

    public:
        vec3 center0, center1;
        double time0, time1;
        double radius;
        shared_ptr<material> mat_ptr;
};

vec3 moving_sphere::center(double time) const
{
    return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
}

bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const 
{
    vec3 oc = r.origin() - center(r.time());
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;

    auto discriminant = half_b*half_b - a*c;

    if (discriminant > 0) 
    {
        auto root = sqrt(discriminant);

        auto temp = (-half_b - root)/a;
        if (temp < t_max && temp > t_min) 
        {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center(r.time())) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }

        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) 
        {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center(r.time())) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

// 对于`moving_sphere`, 我们先求球体在 t0 时刻的包围盒, 再求球体在 t1 时刻的包围盒, 然后再计算这两个盒子的包围盒:
bool moving_sphere::bounding_box(double t0, double t1, aabb& output_box) const 
{
    aabb box0(center(t0) - vec3(radius, radius, radius), center(t0) + vec3(radius, radius, radius));
    aabb box1(center(t1) - vec3(radius, radius, radius), center(t1) + vec3(radius, radius, radius));
    
    // 再求两个包围盒的包围盒
    output_box = surrounding_box(box0, box1);
    return true;
}

#endif



