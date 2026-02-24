// g++ -std=c++26 -freflection
#include <print>
#include <meta>
#include <cstring>
#include <ranges>

using namespace std::meta;

inline constexpr struct{} ignore {};

consteval auto has_annotation(info T, auto anno){
    return !annotations_of_with_type(T, ^^decltype(anno)).empty();
}

template<std::meta::info... NSs>
auto run_tests() -> void {
    template for (auto nsc = 1uz; constexpr auto ns : {NSs...} ){
        std::println("[{}] {}:", nsc, identifier_of(ns));
        template for (auto tstc = 1uz; constexpr auto M : define_static_array(members_of(ns, access_context::current())) ){
            if constexpr (is_function(M)) {
                std::println("  [{}.{}] {} {}",
                    nsc,
                    tstc,
                    identifier_of(M),
                    has_annotation(M, ignore) ? "[ignored]" : ""
                );

                if(!has_annotation(M, ignore)){
                    [:M:]();
                }
            }
            tstc++;
        }
        nsc++;
    }
}

#define expect_eq(x, y)  do{if((x) != (y)) std::println("\t-> [ "#x" == "#y" ] failed. {}:{}",__FILE__, __LINE__);}while(false);
#define expect_ne(x, y)  do{if((x) == (y)) std::println("\t-> [ "#x" != "#y" ] failed. {}:{}",__FILE__, __LINE__);}while(false);
#define expect_streq(x, y)  do{if(std::strcmp(x, y) != 0) std::println("\t-> [ "#x" == "#y" ] failed. {}:{}",__FILE__, __LINE__);}while(false);
#define expect_strne(x, y)  do{if(std::strcmp(x, y) == 0) std::println("\t-> [ "#x" != "#y" ] failed. {}:{}",__FILE__, __LINE__);}while(false);
#define expect_that(statemnt)  do{if(!(statemnt)) std::println("\t-> [ "#statemnt" ] failed. {}:{}",__FILE__, __LINE__);}while(false);
/////////////////////////////////////////////////////////

auto add(int a, int b) -> int {
    return a + b;
}

//namespace as testsuite name
namespace tests {

    void add_random_tests() { // function as test case
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

namespace tests2 {

    void foo() {
        expect_eq(add(2, 2), 4);
        expect_ne(add(2, 2), 5);
    }
}

int main() {
    run_tests<^^tests, ^^tests2>();
}