// g++ -std=c++26 -freflection
#include <print>
#include <meta>

using namespace std::meta;

template <info ns>
void invoke_all() {
    template for (constexpr info M : define_static_array(members_of(ns, access_context::current())))
    {
        if constexpr (is_function(M)) [:M:]();
    }
}

namespace test {
    void test_foo() {
        std::println("Called foo");
    }

    void test_bar() {
        std::println("Called bar");
    }
}

int main() {
    invoke_all<^^test>();
}