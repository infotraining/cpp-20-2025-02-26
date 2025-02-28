#include <catch2/catch_test_macros.hpp>
#include <helpers.hpp>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>
#include <map>

using namespace std::literals;

TEST_CASE("ranges", "[ranges]")
{
    auto data = helpers::create_numeric_dataset<20>(42);
    helpers::print(data, "data");

    std::vector words = {"one"s, "two"s, "three"s, "four"s, "five"s, "six"s, "seven"s, "eight"s, "nine"s, "ten"s, 
                         "eleven"s, "twelve"s, "thirteen"s, "fourteen"s, "fifteen"s, "sixteen"s, "seventeen"s, "eighteen"s, "nineteen"s, "twenty"s};
    helpers::print(words, "words");

    SECTION("algorithms")
    {
        std::sort(words.begin(), words.end());
        std::ranges::sort(words, std::greater{}, [](const auto& s) { return s.size(); });
        CHECK(std::ranges::is_sorted(words, std::greater{}, [](const auto& s) { return s.size(); }));
    }

    SECTION("projections")
    {
        std::vector<std::string> long_words;

        std::ranges::copy_if(words, std::back_inserter(long_words), [](size_t lenght) { return lenght > 3; },
            [](const std::string& w) { return w.size(); });
        helpers::print(long_words, "long_words");
    }

    SECTION("concepts & tools")
    {
        int vec[] = {1, 2, 3, 4};

        using T = std::ranges::range_value_t<decltype(vec)>;
    }
}

template <auto Value>
struct EndValue
{
    bool operator==(auto it) const
    {
        return *it == Value;
    }
};

TEST_CASE("sentinels", "[ranges]")
{
    std::vector data = {2, 3, 4, 1, 5, 42, 6, 7, 8, 9, 10};

    auto pos = std::ranges::find(data.begin(), std::unreachable_sentinel, 42);
    std::sort(data.begin(), pos);   

    std::ranges::sort(data.begin(), EndValue<42>{});

    char txt[] = { 'a', 'b', 'c', '\0', 'e', 'f' };
    std::ranges::sort(txt, EndValue<'\0'>{}, std::greater{});

    helpers::print(data, "data");
}

TEST_CASE("views")
{
    std::vector data = {2, 3, 4, 1, 5, 42, 6, 7, 8, 9, 10};

    SECTION("all")
    {
        auto all_view = std::views::all(data);
        auto target = all_view; // O(1)
    }

    SECTION("subrange - iterator & sentinel as a view")
    {
        auto head = std::ranges::subrange{data.begin(), EndValue<42>{}};
        std::ranges::sort(head);    
        helpers::print(data, "data");

        for(auto& item : head)
            item = 0;

        helpers::print(data, "data");
    }

    SECTION("counted")
    {        
        auto tail = std::views::counted(data.rbegin(), 3);

        helpers::print(tail, "tail");
    }

    SECTION("iota")
    {        
        helpers::print(std::views::iota(1, 10), "iota");
    }

    SECTION("pipes |")
    {
        auto evens = data | std::views::filter([](int x) { return x % 2 == 0; });

        //for(auto it = evens.begin(); it != evens.end(); ++it)
        for(const auto& item : evens)
            std::cout << "even: " << item << "\n";

        auto kroczer = std::views::iota(1)
            | std::views::take(10)
            | std::views::transform([](int n) { return n * n; })
            | std::views::filter([](int x) { return x % 2 == 0; })
            | std::views::reverse
            | std::views::common; // harminizing iterator & sentinel
        
        std::vector vec(kroczer.begin(), kroczer.end());
    }   
}

TEST_CASE("views - reference semantics")
{    
    std::vector data = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto evens_view = data | std::views::filter([](int i) { return i % 2 == 0; });
    helpers::print(data, "data");

    for(auto& even : evens_view)
        even = 0;

    helpers::print(data, "data");
}

TEST_CASE("maps & ranges")
{
    std::map<int, std::string> dict = { {1, "one"}, {2, "two"} };

    auto elems_0 = dict | std::views::elements<0>;

    helpers::print(elems_0, "elems_0");
    helpers::print(dict | std::views::keys, "keys");
    helpers::print(dict | std::views::values, "values");
}