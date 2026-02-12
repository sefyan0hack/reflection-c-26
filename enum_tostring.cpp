// g++ -std=c++26 -freflection
#include <print>
#include <string_view>
#include <meta>

using namespace std::meta;

// form p2996r13 - 3.6 Enum to String
template <typename E>
  requires (is_enum_type(^^E))
constexpr std::string_view enum_to_string(E value) {
  template for (constexpr auto e : std::define_static_array(std::meta::enumerators_of(^^E))) {
    if (value == [:e:]) {
      return identifier_of(e);
    }
  }

  return "<unnamed>";
}

enum Color {
    Red, Green, Yellow
};
enum class State {
    ON, OFF
};

int main()
{
    std::println("{}", enum_to_string(Color::Red)); // Red
    std::println("{}", enum_to_string(State::ON)); // ON
}