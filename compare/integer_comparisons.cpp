#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <numbers>

namespace Explain
{
    //////////////////////////////////////////
    // template variable

    #define M_PI 3.141592653589793238

    template <typename T>
    static constexpr T my_pi = 3.141592653589793238;

    template<>
    static constexpr int my_pi<int> = 4;

    auto MY_2_PI = 2 * static_cast<float>(M_PI);
    auto my_2_pi = 2 * my_pi<float>;

    static_assert(std::cmp_equal(my_pi<int>, 4UL));

    auto r = 10;
    auto area = std::numbers::pi * r * r;

    //////////////////////////////////////////
    // trait - is_void

    template <typename T>
    struct is_void
    {
        static constexpr bool value = false;
    };

    template <>
    struct is_void<void>
    {
        static constexpr bool value = true;
    };

    template <typename T>
    constexpr bool is_void_v = is_void<T>::value;
}

TEST_CASE("traits - how it works")
{
    using T1 = int;
    static_assert(!Explain::is_void_v<T1>);

    using T2 = void;
    static_assert(Explain::is_void<T2>::value);
}

TEST_CASE("safe comparing integral numbers")
{
    SECTION("cmp_less, cmp_*")
    {
        int x = -42;
        unsigned int y = 665;
        CHECK(std::cmp_less(x, y));

        auto my_comparer = [](auto a, auto b) {
            if constexpr(std::is_integral_v<decltype(a)> && std::is_integral_v<decltype(b)>)
            {
                return std::cmp_less(a, b);
            }
            else
            {
                return a < b;
            }
        };
        
        CHECK(my_comparer(x, y));  
        
        uint8_t c1 = 'a';
        int8_t c2 = -1;
        CHECK(my_comparer(c1, c2) == false);
        
        std::string str_a = "abc";
        std::string str_b = "ABC";
        CHECK(my_comparer(str_a, str_b));
    }

    SECTION("in_range")
    {
        int x = 665;
        CHECK(std::in_range<uint8_t>(x) == false);
    }
}