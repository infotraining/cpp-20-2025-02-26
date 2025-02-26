#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std::literals;

struct Point
{
    int x;
    int y;

    friend std::ostream& operator<<(std::ostream& out, const Point& p)
    {
        return out << std::format("Point({},{})", p.x, p.y);
    }

    bool operator==(const Point& other) const = default;
};

struct Point3D : Point
{
    int z;

    constexpr Point3D(int x, int y, int z)
        : Point{x, y}
        , z{z}
    { }

    bool operator==(const Point3D& other) const = default;
};



TEST_CASE("Point - operator ==")
{
    SECTION("Point")
    {
        Point p1{1, 2};
        Point p2{1, 2};
        Point p3{2, 1};

        CHECK(p1 == p2);
        CHECK(p1 != p3); // rewriting - !(p1 == p3)
    }

    SECTION("Point3D")
    {
        constexpr Point3D p1{1, 2, 3};
        constexpr Point3D p2{1, 2, 3};
        Point3D p3{1, 2, 4};

        static_assert(p1 == p2);
        CHECK(p1 != p3);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Comparisons
{

    struct Money
    {
        int dollars;
        int cents;        

        constexpr Money(int dollars, int cents)
            : dollars(dollars)
            , cents(cents)
        {
            if (cents < 0 || cents > 99)
            {
                throw std::invalid_argument("cents must be between 0 and 99");
            }
        }

        constexpr Money(double amount)
            : dollars(static_cast<int>(amount))
            , cents(static_cast<int>(amount * 100) % 100)
        { }

        friend std::ostream& operator<<(std::ostream& out, const Money& m)
        {
            return out << std::format("${}.{}", m.dollars, m.cents);
        }

        auto operator<=>(const Money& other) const = default;

        // bool operator==(const Money& other) const = default;

        // bool operator<(const Money& other) const
        // {
        //     if (dollars == other.dollars)
        //     {
        //         return cents < other.cents;
        //     }

        //     return dollars < other.dollars;
        // }

        // auto operator<=>(const Money& other) const
        // {
        //     if (auto result = dollars <=> other.dollars; result != 0)
        //     {
        //         return cents <=> other.cents;
        //     }
        //     else
        //     {
        //         return result;
        //     }
        // }

        // bool operator==(const Money& other) const = default;
    };

    namespace Literals
    {
        // clang-format off
        constexpr Money operator""_USD(long double amount)
        {
            return Money(amount);
        }
        // clang-format on
    } // namespace Literals
} // namespace Comparisons

TEST_CASE("Money - operator <=>")
{
    using Comparisons::Money;
    using namespace Comparisons::Literals;

    Money m1{42, 50};
    Money m2{42, 50};

    SECTION("comparison operators are synthetized")
    {
        CHECK(m1 == m2);
        CHECK(m1 == Money(42.50));
        CHECK(m1 == 42.50_USD);
        CHECK(m1 != 42.51_USD); // !(m1 == 42.51_USD) - rewriting
        CHECK(m1 < 42.51_USD);  // m1 <=> 42.51_USD < 0
        CHECK(m1 <= 42.51_USD);
        CHECK(m1 > 0.99_USD);
        CHECK(m1 >= 0.99_USD);

        // static_assert(Money{42, 50} == 42.50_USD);
    }

    SECTION("sorting")
    {
        std::vector<Money> wallet{42.50_USD, 13.37_USD, 0.99_USD, 100.00_USD, 0.01_USD};
        std::ranges::sort(wallet);
        CHECK(std::ranges::is_sorted(wallet));
    }
}

TEST_CASE("operator <=>")
{
    SECTION("primitive types")
    {
        SECTION("integral types - strong ordering")
        {
            int x = 42;
            int y = 65;

            auto result = x <=> y;
            CHECK((result == std::strong_ordering::less));
            CHECK((result < 0));            
        }

        SECTION("floating points - partial ordering")
        {
            double dx = 0.01;
            double dy = 0.001;

            auto result = dx <=> dy;
            CHECK((result == std::partial_ordering::greater));
            CHECK((result > 0));

            result = dx <=> std::numeric_limits<double>::quiet_NaN();
            CHECK((result == std::partial_ordering::unordered));
        }

        SECTION("pointers")
        {
            int* a{};
            int* b{};
            auto r = a <=> b;

            auto ptr1 = std::make_unique<int>(42);
            auto ptr2 = std::make_unique<int>(42);
            auto result = ptr1 <=> ptr2;
        }
    }

    SECTION("custom types")
    {
        using namespace Comparisons;
        using namespace Comparisons::Literals;

        SECTION("result is a comparison category")
        {

            auto result = Money(100, 99) <=> 100.99_USD;
        }

        SECTION("operators <, >, <=, >= are synthetized")
        {
            auto result = Money(100, 99) <= 100.99_USD;
        }
    }
}


////////////////////////////////////////////////////////////////////////

struct Temperature
{
    double value;

    // auto operator<=>(const Temperature& other) const
    // {
    //     return std::strong_order(value, other.value);
    // }

    //bool operator==(const Temperature& other) const = default;

    friend bool operator==(const Temperature& left, const Temperature& right) = default;
};

auto operator<=>(const Temperature& left, const Temperature& right)
{
    return std::strong_order(left.value, right.value);
}

TEST_CASE("Temperature - sorting")
{
    std::vector<Temperature> temperatures{Temperature{23.0}, Temperature{std::numeric_limits<double>::quiet_NaN()}, Temperature{62.8}};

    std::sort(temperatures.begin(), temperatures.end());
    std::ranges::sort(temperatures);
    CHECK(std::ranges::is_sorted(temperatures));
}

struct PreCpp20
{
    int value;

    bool operator==(const PreCpp20& other) const
    {
        return value == other.value;
    }

    bool operator<(const PreCpp20& other) const
    {
        return value < other.value;
    }
};

struct PostCpp20
{
    int x;
    PreCpp20 y;

    std::strong_ordering operator<=>(const PostCpp20& other) const = default;
};

TEST_CASE("Pre & Post C++20")
{
    PostCpp20 p1{1, PreCpp20{2}};
    PostCpp20 p2{1, PreCpp20{3}};

    CHECK((p1 <=> p2 < 0));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Comparisons
{
    class Data
    {
        int* buffer_;
        size_t size_;

    public:
        Data(std::initializer_list<int> values)
            : buffer_(new int[values.size()])
            , size_(values.size())
        {
            std::copy(values.begin(), values.end(), buffer_);
        }

        Data(const Data&) = delete;
        Data& operator=(const Data&) = delete;

        ~Data()
        {
            delete[] buffer_;
        }

        auto operator<=>(const Data& other) const
        {
            return std::lexicographical_compare_three_way(buffer_, buffer_ + size_, other.buffer_, other.buffer_ + other.size_);
        }

        bool operator==(const Data& other) const
        {
            if (size_ == other.size_)
                return std::equal(buffer_, buffer_ + size_, other.buffer_);
            return false;
        }        
    };
} // namespace Comparisons

TEST_CASE("lexicographical_compare_three_way")
{
    using Comparisons::Data;

    Data data1{1, 2, 3};
    Data data2{1, 2, 3};
    Data data3{1, 2, 4};

    CHECK(data1 == data2);
    CHECK(data1 < data2);
}