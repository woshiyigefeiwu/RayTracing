// sphere.h������

#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "rtweekend.h"

// ��ͨ����

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

    // �ཻ
    if (discriminant > 0) 
    {
        auto root = sqrt(discriminant);

        // �жϵ�һ����
        auto temp = (-half_b - root)/a;
        if (temp < t_max && temp > t_min) 
        {
            rec.t = temp;
            rec.p = r.at(rec.t);
            // ������������ж�
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            get_sphere_uv((rec.p-center)/radius, rec.u, rec.v);
            return true;
        }

        // �жϵڶ�����
        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) 
        {
            rec.t = temp;
            rec.p = r.at(rec.t);    // ����
            // ������������ж�
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

// ���Χ��
bool sphere::bounding_box(double t0, double t1, aabb& output_box) const 
{
    output_box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
    return true;
}

// -------------------------------------------------------------------------------------------------------

// �˶��е���
// ������������`time0`��`time1`��ʱ����ڴ�`center0`�����˶���`center1`���������ʱ���, ���������Ȼ�ڶ�

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

// ����`moving_sphere`, �������������� t0 ʱ�̵İ�Χ��, ���������� t1 ʱ�̵İ�Χ��, Ȼ���ټ������������ӵİ�Χ��:
bool moving_sphere::bounding_box(double t0, double t1, aabb& output_box) const 
{
    aabb box0(center(t0) - vec3(radius, radius, radius), center(t0) + vec3(radius, radius, radius));
    aabb box1(center(t1) - vec3(radius, radius, radius), center(t1) + vec3(radius, radius, radius));
    
    // ����������Χ�еİ�Χ��
    output_box = surrounding_box(box0, box1);
    return true;
}

#endif



