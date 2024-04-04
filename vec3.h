/* --------------------------------------------------------- 

vec3 类来储存所有的颜色, 位置, 方向, 位置偏移, 或者别的什么东西。

--------------------------------------------------------- */

#include <iostream>
#include <cmath>
#include "rtweekend.h"

class vec3 
{
    public:
        vec3() : e{0,0,0} {}
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }
        
        // 重载运算符
        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        vec3& operator+=(const vec3 &v) 
        {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3& operator*=(const double t) 
        {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(const double t) 
        {
            return *this *= 1/t;
        }

        // 求模
        double length() const 
        {
            return sqrt(length_squared());
        }

        // 求模的平方
        double length_squared() const 
        {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        // 写入颜色值（作为颜色的话，这里做了一下抗锯齿；对于颜色e，是 samples_per_pixel 次采样的累加值，最后写入颜色值的时候取了一下平均）
        void write_color(std::ostream &out, int samples_per_pixel) 
        {
            // Divide the color total by the number of samples and gamma-correct
            // for a gamma value of 2.0.
            auto scale = 1.0 / samples_per_pixel;
            auto r = sqrt(scale * e[0]);
            auto g = sqrt(scale * e[1]);
            auto b = sqrt(scale * e[2]);

            // Write the translated [0,255] value of each color component.
            out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
                << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
                << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
        }

        inline static vec3 random() 
        {
            return vec3(random_double(), random_double(), random_double());
        }

        inline static vec3 random(double min, double max) 
        {
            return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
        }

    public:
        double e[3];
};

inline std::ostream& operator<<(std::ostream &out, const vec3 &v) 
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v) 
{
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) 
{
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) 
{
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) 
{
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t) 
{
    return t * v;
}

inline vec3 operator/(vec3 v, double t) 
{
    return (1/t) * v;
}

inline double dot(const vec3 &u, const vec3 &v) 
{
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) 
{
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// 归一化
inline vec3 unit_vector(vec3 v) 
{
    return v / v.length();
}

// 随机生成一个 [-1,1]^3 内的点
vec3 random_in_unit_sphere() 
{
    while (true) 
    {
        auto p = vec3::random(-1,1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

// 在使用lambertian漫发射模型前, 早期的光线追踪论文中大部分使用的都是这个方法
vec3 random_in_hemisphere(const vec3& normal) 
{
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

// 在圆内生成一个点，然后单位化
vec3 random_unit_vector() 
{
    auto a = random_double(0, 2*pi);
    auto z = random_double(-1, 1);
    auto r = sqrt(1 - z*z);
    return vec3(r*cos(a), r*sin(a), z);
}

// 求反射方向
vec3 reflect(const vec3& v, const vec3& n) 
{
    return v - 2*dot(v,n)*n;
}

/*
    求折射光线
    uv：是入射光线
    n：是法向量
    etai_over_etat：是折射率之比 n / n'
*/
vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) 
{
    auto cos_theta = dot(-uv, n);
    // 折射光线的水平分量
    vec3 r_out_parallel =  etai_over_etat * (uv + cos_theta*n);
    // 折射光线的垂直分量
    vec3 r_out_perp = -sqrt(1.0 - r_out_parallel.length_squared()) * n;
    return r_out_parallel + r_out_perp;
}

// 模拟：发生折射的概率会随着入射角而改变
double schlick(double cosine, double ref_idx) 
{
    auto r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}

// 模拟：从一个单位小圆盘射出光线
vec3 random_in_unit_disk() 
{
    while (true) 
    {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

