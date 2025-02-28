#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <functional>
#include <iostream>
#include <source_location>
#include <string>
#include <vector>
#include <utility>
#include <set>

using namespace std::literals;

struct Person
{
    int id;
    std::string name;
    double salary;

    auto operator<=>(const Person& p) const = default;
};

namespace Explain
{
    template <typename T, typename... TArgs>
    std::unique_ptr<T> make_unique(TArgs&&... args)
    {
        auto loc = std::source_location::current();

        std::cout << "file: " << loc.file_name() << "\n";
        std::cout << "function: " << loc.function_name() << "\n";
        std::cout << "line/col: " << loc.line() << "\n";

        return std::unique_ptr<T>(new T(std::forward<TArgs>(args)...));
    }
} // namespace Explain

TEST_CASE("Aggregates")
{
    SECTION("designated intitializers")
    {
        Person p1{42, "", 10'000.00};
        Person p2{.id = 42, .salary = 10'000.00};

        REQUIRE(p1 == p2);
    }

    SECTION("init with ()")
    {
        Person p1{42, "Kowalski", 10'000.00};
        Person p2(42, "Kowalski", 10'000.00);

        auto ptr_17 = std::make_unique<Person>(Person{42, "Kowalski", 15'000.00});
        auto ptr_20 = Explain::make_unique<Person>(42, "Kowalski", 15'000.00);

        auto lst = {1, 2, 3, 4};
    }
}

template <typename T1, typename T2>
struct Pair
{
    T1 fst;
    T2 snd;
};

// deductions guides - obsolete since C++20
// template <typename T1, typename T2>
// Pair(T1, T2) -> Pair<T1, T2>;

TEST_CASE("CTAD - class template arg deduction")
{
    Pair<int, double> p1(10, 3.14);
    Pair p2(10, 3.14);

    std::pair p3{42, "forty-two"};
    std::vector vec{1, 2, 3, 4};
    std::optional opt = 42;
    std::tuple tpl{1, 3.14, "text"};

    std::function f = [](int n) {
        auto loc = std::source_location::current();

        std::cout << "file: " << loc.file_name() << "\n";
        std::cout << "function: " << loc.function_name() << "\n";
        std::cout << "line/col: " << loc.line() << "\n";

        return n * 2;
    };

    f(42);
}

namespace BeforeCpp17
{
    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_integral<T>::value, bool>
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_floating_point<T>::value, bool>
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == T(0.5);
    }

} // namespace BeforeCpp17

namespace Cpp17
{
    template <typename T>
    bool is_power_of_2(T value)
    {
        if constexpr (std::is_integral_v<T>)
        {
            return value > 0 && (value & (value - 1)) == 0;
        }
        else
        {
            int exponent;
            const T mantissa = std::frexp(value, &exponent);
            return mantissa == T(0.5);
        }
    }
} // namespace Cpp17

namespace Cpp20
{
    template <std::integral T>
    auto is_power_of_2(T value)
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <std::floating_point T>
    auto is_power_of_2(T value)
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == T(0.5);
    }

    // template <BigInt T>
    // auto is_power_of_2(T value)
    // {
    //     ///...
    // }
} // namespace Cpp20

TEST_CASE("constexpr if")
{
    REQUIRE(BeforeCpp17::is_power_of_2(8.0));
    REQUIRE(Cpp17::is_power_of_2(64));
    REQUIRE(Cpp20::is_power_of_2(1024));
}

//////////////////////////////////////////////
// lambda with template parameters

struct Lambda_278345237645
{
    int& factor;

    template <typename T, typename TArg>
    auto operator()(std::vector<T>& vec, TArg&& item) 
    {
        vec.push_back(std::forward<TArg>(item));
    }
};


TEST_CASE("lambda with template parameters")
{
    int factor = 2;
    auto add = [&factor = std::as_const(factor)]<typename T, typename TArg>(std::vector<T>& vec, TArg&& item) mutable -> TArg  {
        vec.push_back(std::forward<TArg>(item) * factor);
        return factor;        
    };

    SECTION("it works like this")
    {
        auto add = Lambda_278345237645{factor};
    }

    std::vector<int> vec;
    add(vec, 42);
    add(vec, 665.44);

    REQUIRE(vec.size() == 2);
    REQUIRE(vec == std::vector{84, 1330});
    REQUIRE(factor == 2);

    auto callback = +[](int n) { std::cout << n << "\n"; };
    callback(42);
}

TEST_CASE("type of lambda")
{
    auto cmp_ptrs = [](const auto& ptr1, const auto& ptr2) { return *ptr1 < *ptr2; };
    auto backup = decltype(cmp_ptrs){};

    std::set<std::shared_ptr<int>, decltype(cmp_ptrs)> ptrs;

    ptrs.insert(std::make_shared<int>(42));
    ptrs.insert(std::make_shared<int>(665));
    ptrs.insert(std::make_shared<int>(2));
    ptrs.insert(std::make_shared<int>(667));

    for(const auto& ptr : ptrs)
        std::cout << *ptr << " ";
    std::cout << "\n";
}

