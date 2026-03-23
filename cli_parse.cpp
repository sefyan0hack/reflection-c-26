#include <string_view>
#include <ranges>
#include <spanstream>
#include <iostream>
#include <vector>
#include <meta>

using namespace std::meta;

template <class Opts>
auto parse(int argc, char** argv) -> Opts {
  std::vector<std::string_view> cmdline(argv+1, argv+argc);

  Opts opts;

  constexpr auto ctx = std::meta::access_context::current();
  template for (constexpr auto opt: std::define_static_array(nonstatic_data_members_of(^^Opts, ctx))) {

    auto it = std::ranges::find_if(cmdline,
      [=](std::string_view arg){
        return (arg.starts_with("--") && arg.substr(2) == identifier_of(opt));
      });

    if(it != cmdline.end())
    {
      if (it + 1 == cmdline.end() || (*(it+1)).starts_with("--")) {
        std::cerr << "Missing value for option " <<  display_string_of(opt) << std::endl;
        std::exit(EXIT_FAILURE);
      }

      auto iss = std::ispanstream(*(it+1));
      if (iss >> opts.[:opt:]; !iss) {
        std::cerr << "Failed to parse `" << *(it+1) << "` to type " <<  display_string_of(type_of(opt)) << std::endl;
        std::exit(EXIT_FAILURE);
      }

      cmdline.erase(it, it+2);
    }
  }
  return opts;
}

struct Options {
  std::string name;
  int count = 1;
};

int main(int argc, char** argv) {
  auto opts = parse<Options>(argc, argv);

  std::cout << "opts.name : " << opts.name << std::endl;
  std::cout << "opts.count : " << opts.count << std::endl;
}