#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std::literals;

struct BoundingBox
{
    int w, h;
};

struct Color
{
    uint8_t r, g, b;
};

struct Rect
{
    int w, h;
    Color color;

    void draw() const
    {
        std::cout << "Rect::draw()\n";
    }

    BoundingBox box() const noexcept
    {
        return BoundingBox{w, h};
    }
};

struct ColorRect : Rect
{
    Color color;

    Color get_color() const noexcept
    {
        return color;
    }

    void set_color(Color new_color)
    {
        color = new_color;
    }
};

// clang-format off
template <typename T>
concept Shape = requires(const T& obj)
{
    { obj.box() } noexcept -> std::same_as<BoundingBox>;
    obj.draw();
};
// clang-format on

template <typename T>
concept ShapeWithColor = Shape<T> && requires(T&& obj, Color c) {
    obj.set_color(c);
    { obj.get_color() } noexcept -> std::same_as<Color>;
};

static_assert(Shape<Rect>);
static_assert(Shape<ColorRect>);
static_assert(ShapeWithColor<ColorRect>);
static_assert(!ShapeWithColor<Rect>);

template <Shape T>
void render(T& shp)
{
    std::cout << "render<Shape T>\n";
    shp.draw();
}

template <ShapeWithColor T>
void render(T& shp)
{
    std::cout << "render<Shape T>\n";
    shp.set_color({255, 255, 255});
    shp.draw();
}

TEST_CASE("concept subsumation")
{
    Rect r{10, 20, {255, 0, 0}};
    ColorRect cr{10, 20, {0, 255, 0}};

    render(r);
    render(cr);
}

////////////////////////////////////

template <typename T>
struct ValueType
{
    using value_type = typename T::value_type;
};

template <typename T>
struct ValueType<T*>
{
    using value_type = T;
};

template <typename T>
using ValueType_t = typename ValueType<T>::value_type;

template <typename T>
concept MyIterator = requires(T it) {
    *it;
    typename ValueType<T>;
};

template <MyIterator It>
ValueType_t<It> algorithm(It iterator)
{
    ValueType_t<It> var = *iterator;
    return var;
}

TEST_CASE("typename in concepts")
{
    using IntPtr = int*;
    static_assert(std::is_same_v<ValueType<IntPtr>::value_type, int>);

    int tab[3] = {1, 2, 3};

    int v1 = algorithm(std::begin(tab));
}

template <typename T>
concept Coutable = requires(T&& obj) {
    std::cout << obj;
};

template <typename... TArgs>
auto sum(TArgs... args) // sum(1, 2, 3, 4, 5)
{
    //return ((((1 + 2) + 3) + 4) + 5);
    return (... + args);  // fold-expression
}

template <Coutable... TArgs>
void print(const TArgs&... arg)
{
    (..., (std::cout << arg << " ")) << "\n"; // fold expressions - C++17

    // (
    //     std::cout << 1 << " ",
    //     std::cout << 3.14 << " ",
    //     std::cout << "text" << " "
    // ) << "\n";
}

TEST_CASE("variadic tempaltes & concepts")
{
    print(1, 3.14, "text");
}