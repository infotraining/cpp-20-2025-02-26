#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

///////////////////////////////////
// trait

template <typename T>
struct IsPointer
{
    constexpr static bool value = false;
};

template <typename T>
struct IsPointer<T*>
{
    constexpr static bool value = true;
};

template <typename T>
constexpr bool IsPointer_v = IsPointer<T>::value;

using namespace std::literals;

template <typename TContainer>
void print(TContainer&& c, std::string_view prefix = "items")
{
    std::cout << prefix << ": [ ";
    for (const auto& item : c)
        std::cout << item << " ";
    std::cout << "]\n";
}

namespace Ver_1
{
    template <typename T>
    T max_value(T a, T b)
    {
        return a < b ? b : a;
    }

    template <typename T>
        requires IsPointer_v<T>
    auto max_value(T a, T b)
    {
        assert(a != nullptr);
        assert(b != nullptr);
        return *a < *b ? *b : *a;
    }
} // namespace Ver_1

inline namespace ver_2
{
    auto max_value(auto a, auto b)
    {
        if constexpr (std::is_integral_v<decltype(a)> && std::is_integral_v<decltype(b)>)
            return std::cmp_less(a, b) ? b : a;
        else
            return a < b ? b : a;
    }

    auto max_value(auto a, auto b)
        requires(std::is_same_v<decltype(a), decltype(b)>
            && std::is_pointer_v<decltype(a)>)
    {
        assert(a != nullptr);
        assert(b != nullptr);
        return *a < *b ? *b : *a;
    }
} // namespace ver_2

///////////////////////////////////////////
// concept

template <typename T>
concept Pointer = IsPointer_v<T>;

static_assert(Pointer<int*> == true);
static_assert(Pointer<int> == false);

namespace Ver_3
{
    template <typename T>
    T max_value(T a, T b)
    {
        return a < b ? b : a;
    }

    template <Pointer T> // template <typename T> requires Pointer<T>
    auto max_value(T a, T b)
    {
        assert(a != nullptr);
        assert(b != nullptr);
        return *a < *b ? *b : *a;
    }
} // namespace Ver_3

namespace ver_4
{
    inline namespace ver_2
    {
        auto max_value(auto a, auto b)
        {
            if constexpr (std::is_integral_v<decltype(a)> && std::is_integral_v<decltype(b)>)
                return std::cmp_less(a, b) ? b : a;
            else
                return a < b ? b : a;
        }

        auto max_value(Pointer auto a, Pointer auto b)
            requires(std::is_same_v<decltype(a), decltype(b)>)
        {
            assert(a != nullptr);
            assert(b != nullptr);
            return *a < *b ? *b : *a;
        }
    } // namespace ver_2
} // namespace ver_4

namespace Ver_5
{
    template <typename T>
    concept Pointer = requires(T ptr) {
        *ptr;
        ptr == nullptr;
    };

    template <typename T>
    T max_value(T a, T b)
    {
        return a < b ? b : a;
    }

    template <Pointer T> // template <typename T> requires Pointer<T>
    auto max_value(T a, T b)
        requires requires { *a < *b; }
    {
        assert(a != nullptr);
        assert(b != nullptr);
        return *a < *b ? *b : *a;
    }
} // namespace Ver_5

TEST_CASE("constraints")
{
    std::integral auto x = 42;
    int y = 20;
    CHECK(max_value(x, y) == 42);

    auto result = max_value(&x, &y);
    CHECK(max_value(&x, &y) == 42);

    auto sp1 = std::make_shared<int>(42);
    auto sp2 = std::make_shared<int>(65);

    CHECK(Ver_5::max_value(sp1, sp2) == 65);
}

std::convertible_to<size_t> auto hash(const auto& value) // std::convertible_to<size_t, ???>
{
    return 425345345UL;
}

template <typename T, std::unsigned_integral auto N>
struct Array
{
    T items[N];
};

template <typename T>
struct Wrapper
{
    T v;

    const T& value() const
    {
        return v;
    }

    decltype(auto) operator[](size_t index)
        requires requires { v[index]; }
    {
        return v[index];
    }
};

TEST_CASE("concepts")
{
    Wrapper wrapped_int{42};
    REQUIRE(wrapped_int.value() == 42);

    Wrapper wrapped_vec{std::vector{1, 2, 3}};
    REQUIRE(wrapped_vec[0] == 1);
}

namespace RequiresExpression
{
    template <typename TContainer>
    decltype(auto) get_nth(TContainer& container, size_t index)
    {
        constexpr bool has_indexing = requires(size_t n) { container[n]; };

        if constexpr (has_indexing)
        {
            std::cout << "container has indexing\n";
            return container[index];
        }
        else
        {
            std::cout << "container does not have indexing. slow loop.\n";
            auto pos = std::begin(container);
            for (size_t i = 0U; i < index; ++i, ++pos)
            { };

            return *pos;
        }
    }
} // namespace RequiresExpression

template <typename T>
concept Indexable = requires(T&& container, size_t n) { container[n]; };

template <typename TContainer>
decltype(auto) get_nth(TContainer& container, size_t index)
{
    if constexpr (Indexable<TContainer>)
    {
        std::cout << "container has indexing\n";
        return container[index];
    }
    else
    {
        std::cout << "container does not have indexing. slow loop.\n";
        auto pos = std::begin(container);
        for (size_t i = 0U; i < index; ++i, ++pos)
        { };

        return *pos;
    }
}

namespace DifficultToRead
{
    template <typename T>
        requires requires(T x) { x + x; }
    T add(T a, T b)
    {
        return a + b;
    }
} // namespace DifficultToRead

template <typename T>
concept Addable = requires(T x) {
    { x + x } -> std::convertible_to<T>;
};

template <Addable T>
T add(T a, T b)
{
    return a + b;
}

TEST_CASE("requires expression")
{
    std::vector<int> container = {1, 2, 3, 4};

    auto& nth = RequiresExpression::get_nth(container, 2);
    REQUIRE(nth == 3);
}

template <typename T>
concept BigType = requires { requires sizeof(T) > 8; };

template <typename T>
concept SmallType = !BigType<T>;

template <typename T>
concept Coutable = requires(T&& obj) {
    std::cout << obj;
};

template <typename T>
    requires SmallType<T> && Coutable<T>
void pass_by_value(T obj)
{
    std::cout << "obj:" << obj << "\n";
}

TEST_CASE("requires & boolean expression")
{
    pass_by_value('a');
    pass_by_value(42);
    // pass_by_value(std::string{"abc"});
}

struct Data
{
    std::string str;
    std::vector<int> vec;

    Data() = default;

    Data(Data&& source) noexcept
        : str(std::move(source.str))
        , vec(std::move(source.vec))
    {
        std::cout << "Data mv\n";
    }

    Data(const Data& source)
    : str(source.str)
    , vec(source.vec)
    {
        std::cout << "Data cpy\n";
    }
};

template <typename TContainer>
void foo(TContainer& container)
{
    using TValue = std::ranges::range_value_t<TContainer>;
    TValue temp{};
}

TEST_CASE("vector & move")
{
    std::vector<Data> vec;

    std::ranges::range_value_t<decltype(vec)> x;

    for(int i = 0; i < 10; ++i)
    {
        vec.push_back(Data{});
        std::cout << "----------\n";
    }
}