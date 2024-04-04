// material.h：材质抽象类
// 1.生成散射后的光线(或者说它吸收了入射光线)
// 2.如果发生散射, 决定光线会变暗多少(attenuate)

#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hittable_list.h"

class material 
{
    public:
        /*
            散射函数
                r_in：入射光线
                rec：碰撞的信息
                attenuate：散射的衰减系数
                scattered：发生散射后的下一条光线
        */
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

// 漫反射材质
class lambertian : public material 
{
    public:
        lambertian(const vec3& a) : albedo(a) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const 
        {
            // 漫反射光线
            vec3 scatter_direction = rec.normal + random_unit_vector();
            // 漫反射后的光线（确保你的材质在运算光线散射时, 散射光线与入射光线所存在的时间点相同）
            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }

    public:
        vec3 albedo;
};

// 金属材质
class metal : public material 
{
    public:
        metal(const vec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const 
        {
            // 反射方向
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            // 散射的光线
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        vec3 albedo;

        // 模糊系数，用来表示模糊的程度（=0表示不产生模糊）
        // 就是系数越大，金属越有那种磨砂的质感
        double fuzz;
};

// 一个只会发生折射的 绝缘体材质
class dielectric : public material 
{
    public:
        dielectric(double ri) : ref_idx(ri) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const 
        {
            attenuation = vec3(1.0, 1.0, 1.0);
            // 折射率
            double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : (ref_idx);

            vec3 unit_direction = unit_vector(r_in.direction());
            
            double cos_theta = ffmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
            
            // 我们认为光线无法发生折射的时候, 他发生了反射
            if (etai_over_etat * sin_theta > 1.0 )
            {
                vec3 reflected = reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected);
                return true;
            }

            /*
                这里模拟的是：
                    现实世界中的玻璃, 发生折射的概率会随着入射角而改变——从一个很狭窄的角度去看玻璃窗, 它会变成一面镜子。
                
                求一下概率，当随机的数小于这个概率的时候，可以认为它只发生了反射
            */
            double reflect_prob = schlick(cos_theta, etai_over_etat);
            if (random_double() < reflect_prob)
            {
                vec3 reflected = reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected);
                return true;
            }

            // 折射
            vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
            scattered = ray(rec.p, refracted);
            return true;
        }

    public:
        double ref_idx;
};

#endif