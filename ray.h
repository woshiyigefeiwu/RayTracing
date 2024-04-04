#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray 
{
    public:
        ray() {}
        ray(const vec3& origin, const vec3& direction, double time = 0.0)
            : orig(origin), dir(direction), tm(time)
        {}

        vec3 origin() const    { return orig; }
        vec3 direction() const { return dir; }
        double time() const { return tm; }

        // 返回光线在t时刻的位置
        vec3 at(double t) const 
        {
            return orig + t*dir;
        }

    public:
        vec3 orig;      // 起始点
        vec3 dir;       // 方向
        double tm;
};

#endif