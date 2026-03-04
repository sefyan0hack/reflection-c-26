// g++ -std=c++26 -freflection
#include <cstdio>
#include <meta>
#include <cstring>
#include <ranges>
#include <generator>


namespace tests {
    using Test = std::generator<const char*>;
}

consteval auto inner_namespaces(std::meta::info namesp) {
    using namespace std::meta;
    return members_of(namesp, access_context::current())
        | std::views::filter(is_namespace)
        | std::ranges::to<std::vector>();
}

template<std::meta::info namesp>
auto run_tests() -> void {
    using namespace std::meta;

    std::puts("+++++++++++++++Unit Tests+++++++++++++++");
    auto testsuite_count = 1uz;
    auto testcase_count = 1uz;
    template for (constexpr auto test_suite : [:reflect_constant_array(inner_namespaces(namesp)):]){
        auto suite_id = identifier_of(test_suite);
        std::printf("%d) %.*s:\n", testsuite_count, static_cast<int>(suite_id.length()), suite_id.data());

        template for (constexpr auto test_case : [:reflect_constant_array(members_of(test_suite, access_context::current())):]){
            auto case_id = identifier_of(test_case);
            if constexpr (is_function(test_case)) {
                std::printf("  %d.%d)  %.*s",
                    testsuite_count,
                    testcase_count,
                    static_cast<int>(case_id.length()), case_id.data()
                );

                int failed = 0;
                for(auto e : [:test_case:]()){
                    failed++;
                    if(failed == 1) std::puts(" [failed]");
                    std::printf("\t%d %s\n", failed, e);
                }
                if(!failed) std::puts(" [passed]");

                testcase_count++;
            }
        }
        testsuite_count++;
        std::puts("\n");
    }

    std::puts("+++++++++++++++++++++++++++++++++++++++++");
    std::printf("%d test suites\n", testsuite_count -1 );
    std::printf("%d test cases\n", testcase_count -1 );
}

// main before the tests cases functions works ?? 
int main() {
    run_tests<^^tests>();
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define expect_eq(x, y) do{if((x) != (y)) co_yield "-> [ "#x" == "#y" ] failed. " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_ne(x, y) do{if((x) == (y)) co_yield "-> [ "#x" != "#y" ] failed. " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_streq(x, y) do{if(std::strcmp(x, y) != 0) co_yield "-> [ "#x" == "#y" ] failed. " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_strne(x, y) do{if(std::strcmp(x, y) == 0) co_yield "-> [ "#x" != "#y" ] failed. " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_true(statemnt) do{if(!(statemnt)) co_yield "-> [ "#statemnt" ] failed. " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_false(statemnt) do{if((statemnt)) co_yield "-> [ "#statemnt" ] failed. " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_any_throw(statemnt) do{ static bool ____i__ = flase; try { statment } catch(...) { ____i__= true; } \
                    if(!____i__) co_yield "-> [ `"#statemnt"` not throwing ]  " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_throw(statemnt, type) do{ static bool ____i__ = flase; try { statment } catch(const type e) { ____i__= true; } \
                    if(!____i__) co_yield "-> [ `"#statemnt"` not throwing a `"#type"` ]  " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_not_any_throw(statemnt) try { statment } catch(...) { co_yield "-> [ `"#statemnt"` is throwing ]  " __FILE__ ":" TOSTRING(__LINE__); }
#define expect_not_throw(statemnt, type) try { statment } catch(const type e) { co_yield "-> [ `"#statemnt"` is throwing a `"#type"`]  " __FILE__ ":" TOSTRING(__LINE__); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

auto add(int a, int b) -> int {
    return a + b;
}

auto mul(int a, int b) -> int {
    return a * b;
}

// namespace as testsuite
namespace tests::addition {

    Test add_random_tests() { // function as test case
        expect_eq(add(2, 2), 4);
        expect_ne(add(2, 2), 5);

        //should fail
        expect_strne("hh", "hh");
        expect_true(add(2, 2) == 5);
    }

    Test add_hundred() {
        expect_eq(add(100, 2), 102);
        expect_eq(add(2, 100), 102);
    }

}

namespace tests::multiplication {

    Test mul_number_by_1() {
        expect_eq(mul(1, 1), 1);
        expect_eq(add(2, 1), 2);

        //should fail
        expect_eq(add(2, 1), 0);

    }

}
