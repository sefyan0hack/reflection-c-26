// g++ -std=c++26 -freflection
#include <print>
#include <meta>
#include <cstring>
#include <ranges>
#include <map>

namespace meta = std::meta;
inline constexpr struct{} ignore {};
namespace tests{}

consteval auto has_annotation(meta::info T, auto anno) -> bool {
    return !meta::annotations_of_with_type(T, ^^decltype(anno)).empty();
}

consteval auto ignored_test(meta::info test) -> bool {
    return has_annotation(test, ignore);
}

consteval auto inner_namespaces(meta::info namesp) {
    return meta::members_of(namesp, meta::access_context::current())
        | std::views::filter([](std::meta::info i) { return meta::is_namespace(i); })
        | std::ranges::to<std::vector>();
}

template<meta::info namesp>
auto run_tests() -> void {
    std::println("+++++++++++++++Unit Tests+++++++++++++++");
    auto testsuite_count = 1uz;
    auto testcase_count = 1uz;
    template for (constexpr auto test_suite : [:meta::reflect_constant_array(inner_namespaces(namesp)):]){

        std::println("{}) {}:", testsuite_count, meta::identifier_of(test_suite));

        template for (constexpr auto test_case : [:meta::reflect_constant_array(meta::members_of(test_suite, meta::access_context::current())):]){
            if constexpr (meta::is_function(test_case)) {
                std::println("  {}.{}) {} {}",
                    testsuite_count,
                    testcase_count,
                    meta::identifier_of(test_case),
                    ignored_test(test_case) ? "[ignored]" : ""
                );

                if constexpr (!ignored_test(test_case)){
                    [:test_case:]();
                }
            }
            testcase_count++;
        }
        testsuite_count++;
        std::println();
    }

    std::println("+++++++++++++++++++++++++++++++++++++++++");
    std::println("{} test suites", testsuite_count -1 );
    std::println("{} test cases", testcase_count -1 );
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

auto mul(int a, int b) -> int {
    return a * b;
}

//namespace as testsuite
namespace tests::adding {

    void add_random_tests() { // function as test case
        expect_eq(add(2, 2), 4);
        expect_ne(add(2, 2), 5);

        //should fail
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

namespace tests::multiplication {

    void mul_number_by_1() {
        expect_eq(mul(1, 1), 1);
        expect_eq(add(2, 1), 2);

        //should fail
        expect_eq(add(2, 1), 0);

    }

}

int main() {
    run_tests<^^tests>();
}