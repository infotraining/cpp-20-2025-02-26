#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <ranges>
#include <algorithm>
#include <numeric>

using namespace std::literals;

int runtime_func(int x)
{
    return x * x;
}

constexpr int constexpr_func(int x)
{
    return x * x;
}

consteval int consteval_func(int x)
{
    return x * x;
}

TEST_CASE("consteval")
{
    int x = 10;
    const int y = 20;
    constexpr int z = 30;

    REQUIRE(runtime_func(10) == 100);
    static_assert(consteval_func(z) == 900);

    int result = consteval_func(z);

    int squares[] = { consteval_func(1), consteval_func(2), consteval_func(3) };
}

constexpr int len(const char* s)
{
    if (std::is_constant_evaluated()) // C++20
    //if consteval // C++23
    {
        // compile-time friendly code
        int idx = 0;

        while (s[idx] != '\0')
            ++idx;
        return idx;
    }
    else
    {
        return std::strlen(s); // function called at runtime
    }
}

constexpr int memory_safe()
{
    int* dyn_int = new int[10]{42};

    const int temp = dyn_int[0];
    delete[] dyn_int;
    
    return temp;
}

TEST_CASE("constexpr extensions")
{
    static_assert(len("abc") == 3);
    const char* text = "abc";
    CHECK(len(text) == 3);


    constexpr int result = memory_safe();
}

template <size_t N>
consteval auto create_powers()
{
    std::array<uint32_t, N> powers{};

    // std::iota(powers.begin(), powers.end(), 1); // std algorithms are constexpr
    // std::ranges::transform(powers, powers.begin(), [](int x) { return x * x; }); // ranges algorithms are constexpr

    auto squares = std::views::iota(0)
        | std::views::transform([](int x) { return x * x; })
        | std::views::take(N);

    std::ranges::copy(squares, powers.begin());

    return powers;
}

TEST_CASE("lookup table")
{
    constexpr auto squares = create_powers<100>();
}

template <std::ranges::input_range... TRng_>
constexpr auto avg_for_unique(const TRng_&... rng)
{
    using TElement = std::common_type_t<std::ranges::range_value_t<TRng_>...>;

    std::vector<TElement> vec;                            // empty vector
    vec.reserve((rng.size() + ...));                      // reserve a buffer - fold expression C++17
    (vec.insert(vec.end(), rng.begin(), rng.end()), ...); // fold expression C++17

    // sort items
    std::ranges::sort(vec); // std::sort(vec.begin(), vec.end());

    // create span of unique_items
    auto new_end = std::unique(vec.begin(), vec.end());
    std::span unique_items{vec.begin(), new_end};

    // calculate sum of unique items
    auto sum = std::accumulate(unique_items.begin(), unique_items.end(), TElement{});

    return sum / static_cast<double>(unique_items.size());
}

TEST_CASE("constexpr - lookup table")
{
    constexpr auto powers = create_powers<100>();

    constexpr std::array lst1 = {1, 2, 3, 4, 5};
    constexpr std::array lst2 = {5, 6, 7, 8, 9};

    constexpr auto avg = avg_for_unique(lst1, lst2);

    std::cout << "AVG: " << avg << "\n";
}