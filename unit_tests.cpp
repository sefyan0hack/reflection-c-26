// g++ -std=c++26 -freflection
#include <print>
#include <meta>
#include <cstring>
#include <ranges>
#include <thread>

inline constexpr struct{} ignore {};

namespace tests{}

consteval auto has_annotation(std::meta::info i, std::meta::info anno) -> bool {
    using namespace std::meta;
    try {
        for( auto annotation : annotations_of(i))
            if(annotation == anno) return true;
    } catch(...) {}
    return false;
}

consteval auto ignored_test(std::meta::info test) -> bool {
    return has_annotation(test, ^^decltype(ignore));
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

    std::println("+++++++++++++++Unit Tests+++++++++++++++");
    auto testsuite_count = 1uz;
    auto testcase_count = 1uz;
    template for (constexpr auto test_suite : [:reflect_constant_array(inner_namespaces(namesp)):]){

        std::println("{}) {}:", testsuite_count, identifier_of(test_suite));

        template for (constexpr auto test_case : [:reflect_constant_array(members_of(test_suite, access_context::current())):]){
            if constexpr (is_function(test_case)) {
                std::print("  {}.{}) {} {}",
                    testsuite_count,
                    testcase_count,
                    identifier_of(test_case),
                    ignored_test(test_case) ? "[ignored]" : ""
                );

                if constexpr (!ignored_test(test_case)){
                    bool failed = false;
                    try {
                        [:test_case:]();
                    } catch (const char* e) {
                        failed = true;
                        std::println("[failed]");
                        std::println("{}", e);
                    } catch (...) {
                        std::println("thread caught unknown exception");
                    }
                    if(!failed) std::println("[passed]");
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

// main before the tests cases functions works ?? 
int main() {
    run_tests<^^tests>();
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define expect_eq(x, y) do{if((x) != (y)) throw "\t-> [ "#x" == "#y" ] failed. " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_ne(x, y) do{if((x) == (y)) throw "\t-> [ "#x" != "#y" ] failed. " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_streq(x, y) do{if(std::strcmp(x, y) != 0) throw "\t-> [ "#x" == "#y" ] failed. " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_strne(x, y) do{if(std::strcmp(x, y) == 0) throw "\t-> [ "#x" != "#y" ] failed. " __FILE__ ":" TOSTRING(__LINE__); } while(false);
#define expect_that(statemnt) do{if(!(statemnt)) throw "\t-> [ "#statemnt" ] failed. " __FILE__ ":" TOSTRING(__LINE__); } while(false);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

auto add(int a, int b) -> int {
    return a + b;
}

auto mul(int a, int b) -> int {
    return a * b;
}

// namespace as testsuite
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
