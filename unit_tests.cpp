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
    constexpr char const* COLOR_RED    = "\x1b[31m";
    constexpr char const* COLOR_GREEN  = "\x1b[32m";
    constexpr char const* COLOR_YELLOW = "\x1b[33m";
    constexpr char const* COLOR_BOLD   = "\x1b[1m";
    constexpr char const* COLOR_RESET  = "\x1b[0m";
    constexpr std::string_view SEP = "========================================";

    std::puts("");
    std::printf("%s%s%s\n", COLOR_BOLD, SEP.data(), COLOR_RESET);
    std::printf("%s  UNIT TESTS (auto-discovered)  %s\n", COLOR_BOLD, COLOR_RESET);
    std::printf("%s%s%s\n\n", COLOR_BOLD, SEP.data(), COLOR_RESET);

    std::size_t total_suites = 0;
    std::size_t total_cases  = 0;
    std::size_t total_failed = 0;
    template for (constexpr auto test_suite : [:reflect_constant_array(inner_namespaces(namesp)):]) {
        constexpr auto suite_id = identifier_of(test_suite);
        total_suites++;
        std::printf("%s%zu) %.*s%s\n",
                    COLOR_BOLD,
                    total_suites,
                    static_cast<int>(suite_id.length()), suite_id.data(),
                    COLOR_RESET);

        template for (constexpr auto test_case : [:reflect_constant_array(members_of(test_suite, access_context::current())):]) {
            constexpr auto case_id = identifier_of(test_case);
            if constexpr (is_function(test_case)) {
                total_cases++;
                std::printf("  %zu.%zu) %.*s ... ",
                            total_suites,
                            total_cases,
                            static_cast<int>(case_id.length()), case_id.data());

                std::size_t failed = 0;
                for (auto e : [:test_case:]()) {
                    failed++;
                    if (failed == 1) {
                        std::printf("%s[failed]%s\n", COLOR_RED, COLOR_RESET);
                    }
                    std::printf("\t%s%d) %s%s\n", COLOR_YELLOW, failed, e, COLOR_RESET);
                }

                if (!failed) {
                    std::printf("%s[passed]%s\n", COLOR_GREEN, COLOR_RESET);
                }else {
                    total_failed++;
                }
            }
        }

        std::puts("");
    }

    // Summary footer
    std::printf("%s%s%s\n", COLOR_BOLD, SEP.data(), COLOR_RESET);
    const char* fail_color = total_failed ? COLOR_RED : COLOR_GREEN;
    std::printf("Suites: %zu   Cases: %zu   %sFailures: %zu%s\n",
                total_suites,
                total_cases,
                fail_color,
                total_failed,
                COLOR_RESET);
    std::printf("%s%s%s\n", COLOR_BOLD, SEP.data(), COLOR_RESET);
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
#define expect_true(statment) do{if(!(statment)) co_yield "-> [ "#statment" ] failed. " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_false(statment) do{if((statment)) co_yield "-> [ "#statment" ] failed. " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_any_throw(statment) do{ static bool ____i__ = false; try { statment } catch(...) { ____i__= true; } \
                    if(!____i__) co_yield "-> [ `"#statment"` not throwing ]  " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_throw(statment, type) do{ static bool ____i__ = false; try { statment } catch(const type e) { ____i__= true; } \
                    if(!____i__) co_yield "-> [ `"#statment"` not throwing a `"#type"` ]  " __FILE__ ":" TOSTRING(__LINE__); } while(false);
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

namespace tests::testingframework {

    Test expect_eq_pass() {
        expect_eq(1, 1);
    }

    Test expect_eq_fail() {
        expect_eq(1, 0);
    }

    Test expect_ne_pass() {
        expect_ne(1, 0);
    }

    Test expect_ne_fail() {
        expect_ne(1, 1);
    }

    Test expect_streq_pass() {
        expect_streq("hello", "hello");
    }

    Test expect_streq_fail() {
        expect_streq("hello", "bey");
    }

    Test expect_strne_pass() {
        expect_strne("hello", "bey");
    }

    Test expect_strne_fail() {
        expect_strne("hello", "hello");
    }

    Test expect_true_pass() {
        expect_true(1 == 1);
    }

    Test expect_true_fail() {
        expect_true(1 != 1);
    }

    Test expect_false_pass() {
        expect_false(1 != 1);
    }

    Test expect_false_fail() {
        expect_false(1 == 1);
    }

    Test expect_any_throw_pass() {
        expect_any_throw( { throw 1; } );
    }

    Test expect_any_throw_fail() {
        expect_any_throw( {int a = 0;} );
    }

    Test expect_throw_pass() {
        expect_throw( { throw 1; }, int );
    }

    Test expect_throw_fail() {
        expect_throw( {int a = 0;}, int );
    }

}
