#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace std::literals;

void print(std::span<const int> data, std::string_view desc)
{
    std::cout << desc << ": [ ";
    for(const auto& item : data)
        std::cout << item << " ";
    std::cout << "]\n";
}

void zero(std::span<int> data, int default_value = 0)
{
    for(auto& item : data)
        item = default_value;
}

struct Data
{
    std::vector<char> buffer_;

    std::span<char> get_buffer()
    {
        return {buffer_};
    }
};

TEST_CASE("std::span")
{
    std::vector vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    print(vec, "vec");

    std::span all_item{vec};
    std::span head = all_item.first(3);
    zero(head);
    print(vec, "vec");  // UB
}

void print_as_bytes(const float f, const std::span<const std::byte> bytes)
{
#ifdef __cpp_lib_format
    std::cout << std::format("{:+6}", f) << " - { ";

    for (std::byte b : bytes)
    {
        std::cout << std::format("{:02X} ", std::to_integer<int>(b));
    }

    std::cout << "}\n";
#endif
}

TEST_CASE("float as span of bytes")
{
    float data[] = {std::numbers::pi_v<float>};

    std::span<const std::byte> const_bytes = std::as_bytes(std::span{data});
    print_as_bytes(data[0], const_bytes);

    std::span<std::byte> writeable_bytes = std::as_writable_bytes(std::span{data});
    writeable_bytes[3] |= std::byte{0b1000'0000};
    print_as_bytes(data[0], const_bytes);
}

//////////////////////////////////////////////////
// BEWARE

constexpr std::span<int> get_head(std::span<int> items, size_t head_size = 1)
{
    return items.first(head_size);
}

TEST_CASE("beware of dangling pointers")
{
    SECTION("OK")
    {
        std::vector vec = {1, 2, 3, 4, 5};
        auto head = get_head(vec, 3);
        print(head, "head");
    }

    SECTION("dangling pointers")
    {
        std::vector vec = {1, 2, 3, 4, 5};
        auto head = get_head(vec, 3);
        vec.push_back(6);
        print(head, "head");
    }
}