// g++ -std=c++26 -freflection
#include <print>
#include <meta>
#include <ranges>
#include <cassert>

using namespace std::meta;

inline constexpr struct{} test {};
inline constexpr struct{} ignore {};
consteval auto has_annotation(info T, info anno){
    return !annotations_of_with_type(T, anno).empty();
}
/////////////////////////////////////////////////////////

auto add(int a, int b) -> int {
    return a + b;
}

namespace tests {
    [[=test]]
    auto test_add() -> void {
        assert(add(2, 2) == 4);
    }

    [[=test]]
    auto test_add_hundred() -> void {
        assert(add(100, 2) == 102);
        assert(add(2, 100) == 102);
    }

    [[=test]]
    [[=ignore]]
    auto ignored_test() -> void {
        assert(add(0, 0) == 0);
    }
}

int main() {
    template for (auto i = 0uz; constexpr info M : define_static_array(members_of(^^tests, access_context::current())))
    {
        if constexpr (is_function(M)) {
            if constexpr (has_annotation(M, ^^decltype(test))){
                std::println("test #{} : {} {}",
                    i++,
                    display_string_of(M),
                    has_annotation(M, ^^decltype(ignore)) ? "[ignored]" : ""
                );
            }
        }
    }
}