#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std::literals;

void foo(auto param1, auto param2)
{
    std::cout << "foo(" << param1 << ", " << param2 << ")\n";
}

// template <>
// void foo<int, double>(int n, double dx)
// {
//     std::cout << "foo(" << n << ", " << dx << ")n";
// }

bool cmp_by_value(auto ptr1, auto ptr2)
{
    assert(ptr1 != nullptr);
    assert(ptr2 != nullptr);
    return *ptr1 < *ptr2;
}

auto cmp_by_value_lambda = [](auto ptr1, auto ptr2)
{
    assert(ptr1 != nullptr);
    assert(ptr2 != nullptr);
    return *ptr1 < *ptr2;
};

TEST_CASE("function & auto params")
{
    foo<int, double>(42, 3.14);
    foo("abc", 665);
    foo("abc"s, 3.14);

    std::vector ptrs = { std::make_shared<int>(42), std::make_shared<int>(1), std::make_shared<int>(665) };
    std::ranges::sort(ptrs, cmp_by_value_lambda);
}

TEST_CASE("templates & lambda expressions")
{
    REQUIRE(true);
}