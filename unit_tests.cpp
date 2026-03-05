// g++ -std=c++26 -freflection
#include <cstdio>
#include <meta>
#include <cstring>
#include <ranges>
#include <generator>

namespace tests {
    using Test = std::generator<const char*>;
}
namespace testing_framework {

    constexpr char const* COLOR_RED    = "\x1b[31m";
    constexpr char const* COLOR_GREEN  = "\x1b[32m";
    constexpr char const* COLOR_YELLOW = "\x1b[33m";
    constexpr char const* COLOR_BOLD   = "\x1b[1m";
    constexpr char const* COLOR_RESET  = "\x1b[0m";
    constexpr std::string_view SEP = "========================================";
    constexpr auto ctx = std::meta::access_context::unprivileged();

    consteval auto inner_namespaces(std::meta::info namesp) {
        using namespace std::meta;
        return members_of(namesp, ctx)
            | std::views::filter(is_namespace)
            | std::ranges::to<std::vector>();
    }
    consteval auto is_test_case(std::meta::info t) -> bool { return is_function(t) && std::meta::return_type_of(t) == ^^tests::Test; }

    template<std::meta::info namesp = ^^::tests>
    auto print_tests() -> void {
        using namespace std::meta;

        template for (constexpr auto test_suite : [:reflect_constant_array(inner_namespaces(namesp)):]) {
            constexpr auto suite_id = identifier_of(test_suite);
            template for (constexpr auto test_case : [:reflect_constant_array(members_of(test_suite, ctx)):]) {
                if constexpr (is_test_case(test_case)) {
                    constexpr auto case_id = identifier_of(test_case);
                    std::printf("%.*s.%.*s\n",
                        static_cast<int>(suite_id.length()), suite_id.data(),
                        static_cast<int>(case_id.length()), case_id.data()
                    );
                }
            }
        }
    }

    template<std::meta::info testcase> requires (is_test_case(testcase))
    auto run() -> bool {
        using namespace std::meta;
        constexpr auto case_id = identifier_of(testcase);

        std::printf("%.*s ... ",
            static_cast<int>(case_id.length()), case_id.data()
        );

        std::size_t failed = 0;
        for (auto e : [:testcase:]()) {
            failed++;
            if (failed == 1) {
                std::printf("%s[failed]%s\n", COLOR_RED, COLOR_RESET);
            }
            std::printf("\t%s%d) %s%s\n", COLOR_YELLOW, failed, e, COLOR_RESET);
        }

        if (!failed) {
            std::printf("%s[passed]%s\n", COLOR_GREEN, COLOR_RESET);
            return true;
        }

        return false;
    }

    template<std::meta::info namesp = ^^::tests> requires (is_namespace(namesp))
    auto run_test(std::string_view test) -> bool {
        using namespace std::meta;

        auto pos = test.find('.');
        if (pos == std::string_view::npos)
            throw std::runtime_error("Invalid test format");

        auto tsuite = test.substr(0, pos);
        auto tcase  = test.substr(pos + 1);

        template for (constexpr auto test_suite : [:reflect_constant_array(inner_namespaces(namesp)):]) {
            constexpr auto suite_id = identifier_of(test_suite);
            template for (constexpr auto test_case : [:reflect_constant_array(members_of(test_suite, ctx)):]) {
                if constexpr (is_test_case(test_case)){
                    constexpr auto case_id = identifier_of(test_case);
                    if (suite_id == tsuite && case_id == tcase) {
                        return run<test_case>();
                    } else {
                        std::printf("`%.*s` not exist ",
                            static_cast<int>(test.length()), test.data()
                        );
                        return false;
                    }
                }
            }
        }
    }
    template<std::meta::info namesp = ^^::tests> requires (is_namespace(namesp))
    auto run_suite(std::string_view suite) -> bool {
        using namespace std::meta;
        
        std::size_t failed = 0;
        bool suite_found = false;
        template for (constexpr auto test_suite : [:reflect_constant_array(inner_namespaces(namesp)):]) {
            constexpr auto suite_id = identifier_of(test_suite);
            if (suite_id == suite) {
                template for (constexpr auto test_case : [:reflect_constant_array(members_of(test_suite, ctx)):]) {
                    if constexpr (is_test_case(test_case)){
                        if(!run<test_case>()) failed++;
                    }
                }
                suite_found = true;
            }
        }

        if (!suite_found) {
            std::printf("`%.*s` not exist ",
                static_cast<int>(suite.length()), suite.data()
            );
        }

        return !suite_found || failed == 0;
    }


    template<std::meta::info namesp = ^^::tests> requires (is_namespace(namesp))
    auto run_all() -> bool {
        using namespace std::meta;

        std::puts("");
        std::printf("%s%s%s\n", COLOR_BOLD, SEP.data(), COLOR_RESET);
        std::printf("%s  UNIT TESTS  %s\n", COLOR_BOLD, COLOR_RESET);
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

            template for (constexpr auto test_case : [:reflect_constant_array(members_of(test_suite, ctx)):]) {
                constexpr auto case_id = identifier_of(test_case);
                if constexpr (is_test_case(test_case)) {
                    total_cases++;
                    std::printf("  %zu.%zu) ", total_suites, total_cases);
                    if(!run<test_case>()) total_failed++;
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

        return total_failed == 0;
    }

    int main(int argc, char** argv) {
        if(argc > 1 && std::strcmp(argv[1], "--list") == 0){
            print_tests();
            return 0;
        } else if( argc > 1 && std::strcmp(argv[1], "--run") == 0) {
            if(!(argc > 2)) {
                std::printf( "provide testsuite.testcase");
                return 1;
            } else {
                if(run_test(std::string_view{argv[2]})) return 0;
                return 1;
            }
        } else if( argc > 1 && std::strcmp(argv[1], "--run-suite") == 0) {
            if(!(argc > 2)) {
                std::printf( "provide testsuite");
                return 1;
            } else {
                if(run_suite(std::string_view{argv[2]})) return 0;
                return 1;
            }
        } else {
            if(argc > 1){
                std::printf("unknown option %s", argv[1]); 
                return 1;
            }
        }

        if(!run_all()) return 1;
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

}

// main before the tests cases functions works ?? 
int main(int argc, char** argv) { testing_framework::main(argc, argv); }
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
