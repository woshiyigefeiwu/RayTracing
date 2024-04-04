// hittable.h：任何可能与光线求交的东西实现时都继承这个类

#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

class material;

// 存一些计算结果
struct hit_record 
{
    vec3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr;
    double t;
    bool front_face;    // 这个是用来判断光线是从外部还是内部打到表面的，true 表示从外部，false 表示从内部

    inline void set_face_normal(const ray& r, const vec3& outward_normal) 
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};

class hittable 
{
    public:
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif