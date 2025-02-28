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


template <double Factor, typename T>
auto scale(T x)
{
    return x * Factor;
}

TEST_CASE("NTTP & floating points")
{
    CHECK(scale<2.0>(8) == 16.0);
}

struct Tax
{
    double value;

    constexpr Tax(double v) : value{v}
    {}
};

template <Tax Vat>
constexpr auto calc_gross_price(double net_price)
{
    return net_price + net_price * Vat.value;
}

TEST_CASE("NTTP & structs")
{
    constexpr Tax vat_pl{0.23};
    constexpr Tax vat_ger{0.19};

    REQUIRE(calc_gross_price<vat_pl>(100.0) == 123.0);
    REQUIRE(calc_gross_price<vat_ger>(100.0) == 119.0);
}

template <size_t N>
struct Str
{
    char text[N];

    constexpr Str(const char(&str)[N])
    {
        std::copy(str, str + N, text);
    }

    friend std::ostream& operator<<(std::ostream& out, const Str& str)
    {
        out << str.text;

        return out;
    }
};

template <Str LogName>
class Logger
{
public:
    void log(std::string_view msg)
    {
        std::cout << LogName << ": " << msg << "\n";
    }
};

TEST_CASE("NTTP & strings")
{
    Logger<"main_logger"> logger_1;
    Logger<"backup_logger"> logger_2;

    logger_1.log("Start");
    logger_2.log("Stop");
}

template <std::invocable auto GetVat>
double calculate_gross_price_with_lambda(double price)
{
    return price + price * GetVat();
}

double get_vat_paradise()
{
    return 0.0;
}

TEST_CASE("NTTP & lambda")
{
    constexpr auto get_vat_pl = [] { return 0.23; };
    auto get_vat_ger = [] { return 0.19; };

    REQUIRE(calculate_gross_price_with_lambda<get_vat_ger>(100.0) == 119.0);
    REQUIRE(calculate_gross_price_with_lambda<[]{ return 0.0; }>(100.0) == 100.0);
    REQUIRE(calculate_gross_price_with_lambda<get_vat_paradise>(100.0) == 100.0);
}