#ifndef BVH_H
#define BVH_H

// 这个就是 BVH 加速结构

#include "hittable.h"
#include "hittable_list.h"
#include "rtweekend.h"
#include <algorithm>

class bvh_node : public hittable 
{
    public:
        bvh_node();

        bvh_node(hittable_list& list, double time0, double time1)
            : bvh_node(list.objects, 0, list.objects.size(), time0, time1)
        {}

        // 再初始化的时候就把 BVH 树 建好
        bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end, double time0, double time1);

        virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
        virtual bool bounding_box(double t0, double t1, aabb& output_box) const;

    public:
        // 注意我们的子节点指针是`hittable*`, 所以这个指针可以指向所有的`hittable`类。例如节点`bvh_node`， 或者是`sphere`, 或者是其他各种各样的图元。
        shared_ptr<hittable> left;
        shared_ptr<hittable> right;

        aabb box;       // 包围盒
};

bool bvh_node::bounding_box(double t0, double t1, aabb& output_box) const 
{
    output_box = box;
    return true;
}

// 判断包围盒是否被击中：检查这个节点的box是否被击中, 如果是的话, 那就对这个节点的子节点进行判断。
bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const 
{
    if (!box.hit(r, t_min, t_max))
        return false;

    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

    return hit_left || hit_right;
}

inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) 
{
    aabb box_a;
    aabb box_b;

    // 如果当前物体没有包围盒
    if (!a->bounding_box(0,0, box_a) || !b->bounding_box(0,0, box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    // 按照指定的轴排序
    return box_a.min().e[axis] < box_b.min().e[axis];
}

bool box_x_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) 
{
    return box_compare(a, b, 0);
}

bool box_y_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) 
{
    return box_compare(a, b, 1);
}

bool box_z_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) 
{
    return box_compare(a, b, 2);
}

// 构造函数，在构造函数中就建好了 bvh 结构
bvh_node::bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end, double time0, double time1)
{
    // 随机一个轴来排序（x y z）
    int axis = random_int(0,2);

    // 函数指针（用来sort()函数的比较）
    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;

    // 元素个数
    size_t object_span = end - start;

    // 只剩下一个物体（左右两个节点都是一样的就行）
    if (object_span == 1)
    {
        left = right = objects[start];
    }
    // 剩下两个物体（）
    else if (object_span == 2) 
    {
        // 小的排前面（从小到大排）
        if (comparator(objects[start], objects[start+1])) 
        {
            left = objects[start];
            right = objects[start+1];
        } 
        else 
        {
            left = objects[start+1];
            right = objects[start];
        }
    } 
    else 
    {
        // 按照选定的轴，从小到大排序物体
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        // 对半分物体，然后递归下去
        auto mid = start + object_span / 2;
        left = make_shared<bvh_node>(objects, start, mid, time0, time1);
        right = make_shared<bvh_node>(objects, mid, end, time0, time1);
    }

    aabb box_left, box_right;

    // 求一下两边物体集合的包围盒
    if (!left->bounding_box (time0, time1, box_left) || !right->bounding_box(time0, time1, box_right))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    box = surrounding_box(box_left, box_right);
}

#endif