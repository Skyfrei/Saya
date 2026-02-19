#ifndef VEC2_H
#define VEC2_H
#include <compare>

class Vec2
{
  public:
    Vec2(int a, int b) {
        x = a;
        y = b;
    }
    Vec2() {
    }

  public:
    constexpr bool operator==(const Vec2 &) const = default;
    constexpr bool operator!=(const Vec2 &) const = default;
    constexpr auto operator<=>(const Vec2 &) const = default;
    constexpr Vec2 operator-(const Vec2& other) const {
        return {x - other.x, y - other.y};
    }

    int x, y;
};

#endif
