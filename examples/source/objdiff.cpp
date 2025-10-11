#include <dtl_modern/dtl_modern.hpp>

#include <vector>

struct Obj1
{
    int value = 0;

    bool operator==(const Obj1&) const = default;
};

struct Obj2
{
    float value = 0.0f;
};

template <typename Obj, typename... Args>
std::vector<Obj> obj_vec(Args... args)
{
    auto vec = std::vector<Obj>{};
    (..., vec.emplace_back(args));
    return vec;
}

int main()
{
    auto a1 = obj_vec<Obj1>(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    auto b1 = obj_vec<Obj1>(3, 5, 1, 4, 5, 1, 7, 9, 6, 10);

    // has equality operator, can omit comparator, or you can override the comparison
    auto unidiff_result1 = dtl_modern::unidiff(a1, b1);

    auto a2 = obj_vec<Obj2>(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    auto b2 = obj_vec<Obj2>(3, 5, 1, 4, 5, 1, 7, 9, 6, 10);

    // don't have equality operator must provide one at call site
    auto unidiff_result2 = dtl_modern::unidiff(a2, b2, [](auto&& l, auto&& r) { return l.value == r.value; });
}
