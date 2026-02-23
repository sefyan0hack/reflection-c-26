// g++ -std=c++26 -freflection
#include <print>
#include <meta>
#include <cstring>

using namespace std::meta;

inline constexpr struct{} ignore {};

consteval auto has_annotation(info T, auto anno){
    return !annotations_of_with_type(T, ^^decltype(anno)).empty();
}

template<info NS>
auto run_tests(){
    template for (auto i = 1uz; constexpr info M : define_static_array(members_of(NS, access_context::current()))){
        if constexpr (is_function(M)) {
            std::println("#{} {} :: {} {}",
                i++,
                identifier_of(NS),
                identifier_of(M),
                has_annotation(M, ignore) ? "[ignored]" : ""
            );

            if(!has_annotation(M, ignore)){
                [:M:]();
            }
        }
    }
}

#define expect_eq(x, y)  do{if((x) != (y)) std::println("\t -> [ "#x" == "#y" ] failed. {}:{}",__FILE__, __LINE__);}while(false);
#define expect_ne(x, y)  do{if((x) == (y)) std::println("\t -> [ "#x" != "#y" ] failed. {}:{}",__FILE__, __LINE__);}while(false);
#define expect_streq(x, y)  do{if(std::strcmp(x, y) != 0) std::println("\t -> [ "#x" == "#y" ] failed. {}:{}",__FILE__, __LINE__);}while(false);
#define expect_strne(x, y)  do{if(std::strcmp(x, y) == 0) std::println("\t -> [ "#x" != "#y" ] failed. {}:{}",__FILE__, __LINE__);}while(false);
#define expect_that(statemnt)  do{if(!(statemnt)) std::println("\t -> [ "#statemnt" ] failed. {}:{}",__FILE__, __LINE__);}while(false);
/////////////////////////////////////////////////////////

auto add(int a, int b) -> int {
    return a + b;
}

namespace tests {

    void add_random_tests() {
        expect_eq(add(2, 2), 4);
        expect_ne(add(2, 2), 5);
        expect_strne("hh", "hh");
        expect_that(add(2, 2) == 5);
    }

    void add_hundred() {
        expect_eq(add(100, 2), 102);
        expect_eq(add(2, 100), 102);
    }

    [[=ignore]]
    void ignored_test() {
        expect_eq(add(0, 0), 0);
    }
}

int main() {
    run_tests<^^tests>();
}