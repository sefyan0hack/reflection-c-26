// g++ -std=c++26 -freflection
#include <print>
#include <meta>
#include <vector>

using namespace std::meta;

template <info ns>
void invoke_all() {
    std::vector<info> test_suites;
    template for (constexpr info M : define_static_array(members_of(ns, access_context::current())))
    {
        if constexpr (is_namespace(M)) test_suites.push_back(M);
        if constexpr (is_function(M)) [:M:]();
    }
}

namespace tests {
    namespace TestSuite {
        void test_case1() {
            std::println("case 1");
        }

        void test_case2() {
            std::println("case 2");
        }
    }
}

int main() {
    invoke_all<^^tests>();
}