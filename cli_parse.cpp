#include <string>
#include <string_view>
#include <ranges>
#include <meta>
#include <spanstream>
#include <print>
#include <array>
#include <vector>

using namespace std::meta;

template <class Spec>
auto parse(int argc, char** argv) -> Spec {
  std::vector<std::string_view> cmdline(argv+1, argv+argc);
  Spec opts;

  constexpr auto ctx = std::meta::access_context::current();
  template for (constexpr auto opt: std::define_static_array(nonstatic_data_members_of(^^Spec, ctx))) {

    auto it = std::ranges::find_if(cmdline,
      [=](std::string_view arg){
        return (arg.starts_with("--") && arg.substr(2) == identifier_of(opt));
      });
    
    if(it != cmdline.end())
    {
      auto iss = std::ispanstream(it[1]);
      if (iss >> opts.[:opt:]; !iss) {
        std::print(stderr, "Failed to parse {:?} into option {} of type {}\n", it[1], display_string_of(opt), display_string_of(type_of(opt)));
        std::exit(EXIT_FAILURE);
      }
    }
  }
  return opts;
}

struct Args {
  std::string name;
  int count = 1;
};

auto print_struct(const auto& obj) -> void
{
  constexpr info T_ref = decay(type_of(^^obj));

  constexpr auto ctx = access_context::current(); 
  std::println("struct {} {{", display_string_of(T_ref));
  template for (constexpr auto a : [:reflect_constant_array(nonstatic_data_members_of(T_ref, ctx)):]) {
    std::println("  {} {} = {} ;", display_string_of(type_of(a)), identifier_of(a), obj.[:a:]);
  }
  std::println("}};");
}

int main(int argc, char** argv) {

  auto opts = parse<Args>(argc, argv);
  print_struct(opts);
}