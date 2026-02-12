// g++ -std=c++26 -freflection
#include <print>
#include <cstdint>
#include <meta>
#include <vector>

using namespace std::meta;
// is a what ever the reflected thing is in std:: namespace ` std::vector,  std::...::...` etc. 
// why not format std:: its `UB`
consteval bool is_in_std_namespace(std::meta::info type) 
{
    constexpr auto std_ns = ^^std;

    auto current = type;

    while (has_parent(current))
    {
        current = parent_of(current);
        if (current == std_ns) return true;
    }

    return false;
}

// form p2996r13 - 3.12 A Universal Formatter (with minor fix/changes)
template <typename T> // <- add template hire to cover the wanted range of types
    requires (is_class_type(^^T) && !is_in_std_namespace(^^T)) // <- is class/struct and not member of std::.* or std::.*::.*  etc
struct std::formatter<T> {
  constexpr auto parse(auto& ctx) { return ctx.begin(); }

  auto format(T const& t, auto& ctx) const {
    auto out = std::format_to(ctx.out(), 
    "{}{{", has_identifier(^^T) ? identifier_of(^^T) : "(unnamed-type)");

    auto delim = [first=true, &out]() mutable { // <- cature out
      if (!first) {
        *out++ = ',';
        *out++ = ' ';
      }
      first = false;
    };

    constexpr auto m_ctx = std::meta::access_context::unchecked(); // rename to m_ctx to resolve conflect

    template for (constexpr auto base : define_static_array(bases_of(^^T, m_ctx))) {
      delim();
      out = std::format_to(out, "{}", (typename [: type_of(base) :] const&)(t));
    }

    template for (constexpr auto mem : define_static_array(nonstatic_data_members_of(^^T, m_ctx)))
    {
      delim();
      std::string_view mem_label = has_identifier(mem) ? identifier_of(mem): "(unnamed-member)";
      out = std::format_to(out, ".{}={}", mem_label, t.[:mem:]);
    }

    *out++ = '}';
    return out;
  }
};

struct Weapon {
    uint32_t  damage;
    uint32_t  ragne;
    uint8_t   health;
};

struct Position {
    float x,y,z;
};

struct Player {
    const char* name;
    uint8_t health;
    std::vector<Weapon>   weapons;
    Position pos;
};

int main(){
    std::println("{}", Player{ 
        "player1",
        70,
        {Weapon { 10, 5, 100 }, Weapon { 20, 2, 70 }},
        Position { 0.0f, 0.0f, 0.0f }
    });  // universal_formatter

    std::println("-------------------------------------------");
    std::println("{}", std::vector{ 1, 2, 3}); // the deafult ranges formatter hire
}


// posible output
// Player{.name=player1, .health=70, .weapons=[Weapon{.damage=10, .ragne=5, .health=100}, Weapon{.damage=20, .ragne=2, .health=70}], .pos=Position{.x=0, .y=0, .z=0}}
// -------------------------------------------
// [1, 2, 3]
