// material.h�����ʳ�����
// 1.����ɢ���Ĺ���(����˵���������������)
// 2.�������ɢ��, �������߻�䰵����(attenuate)

#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hittable_list.h"
#include "texture.h"

class material 
{
    public:
        /*
            ɢ�亯��
                r_in���������
                rec����ײ����Ϣ
                attenuate��ɢ���˥��ϵ��
                scattered������ɢ������һ������
        */
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;

        // ������ߺ���(���ǹ�Դ������д��Ĭ��Ϊ��ɫ)
        virtual vec3 emitted(double u, double v, const vec3& p) const 
        {
            return vec3(0,0,0);
        }
};

// ���������
class lambertian : public material 
{
    public:
        lambertian(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const 
        {
            // ���������
            vec3 scatter_direction = rec.normal + random_unit_vector();
            // �������Ĺ��ߣ�ȷ����Ĳ������������ɢ��ʱ, ɢ�������������������ڵ�ʱ�����ͬ��
            scattered = ray(rec.p, scatter_direction, r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    public:
        shared_ptr<texture> albedo;
};

// ��������
class metal : public material 
{
    public:
        metal(const vec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const 
        {
            // ���䷽��
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            // ɢ��Ĺ���
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        vec3 albedo;

        // ģ��ϵ����������ʾģ���ĳ̶ȣ�=0��ʾ������ģ����
        // ����ϵ��Խ�󣬽���Խ������ĥɰ���ʸ�
        double fuzz;
};

// һ��ֻ�ᷢ������� ��Ե�����
class dielectric : public material 
{
    public:
        dielectric(double ri) : ref_idx(ri) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const 
        {
            attenuation = vec3(1.0, 1.0, 1.0);
            // ������
            double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : (ref_idx);

            vec3 unit_direction = unit_vector(r_in.direction());
            
            double cos_theta = ffmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
            
            // ������Ϊ�����޷����������ʱ��, �������˷���
            if (etai_over_etat * sin_theta > 1.0 )
            {
                vec3 reflected = reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected);
                return true;
            }

            /*
                ����ģ����ǣ�
                    ��ʵ�����еĲ���, ��������ĸ��ʻ���������Ƕ��ı䡪����һ������խ�ĽǶ�ȥ��������, ������һ�澵�ӡ�
                
                ��һ�¸��ʣ����������С��������ʵ�ʱ�򣬿�����Ϊ��ֻ�����˷���
            */
            double reflect_prob = schlick(cos_theta, etai_over_etat);
            if (random_double() < reflect_prob)
            {
                vec3 reflected = reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected);
                return true;
            }

            // ����
            vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
            scattered = ray(rec.p, refracted);
            return true;
        }

    public:
        double ref_idx;
};

// һ�ַ���Ĳ���
class diffuse_light : public material  
{
    public:
        diffuse_light(shared_ptr<texture> a) : emit(a) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const 
        {
            return false;
        }

        virtual vec3 emitted(double u, double v, const vec3& p) const 
        {
            return emit->value(u, v, p);
        }

    public:
        shared_ptr<texture> emit;
};

// ����ɢ��ķ�����˵, ���ǲ��ø���ͬ��(isotropic)�������λ������
class isotropic : public material 
{
    public:
        isotropic(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const  
        {
            scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    public:
        shared_ptr<texture> albedo;
};

#endif