#include <string_view>
#include <ranges>
#include <algorithm>
#include <spanstream>
#include <iostream>
#include <vector>
#include <meta>

using namespace std::meta;

template <typename Char> struct basic_constant_string
{
  using value_type = std::basic_string_view<Char>;

  const Char* data_;
  std::size_t size_;

  template <auto N> constexpr basic_constant_string(const Char (&str)[N])
  {
    const auto ss = std::basic_string_view<Char>(define_static_string(std::basic_string_view<Char>(str, N - 1)));
    data_ = ss.data();
    size_ = ss.size();
  }

  constexpr basic_constant_string(std::basic_string_view<Char> str)
  {
    const auto ss = std::basic_string_view<Char>(define_static_string(str));
    data_         = ss.data();
    size_         = ss.size();
  }

  constexpr basic_constant_string(std::basic_string<Char> str)
  {
    const auto ss =
        std::basic_string_view<Char>(define_static_string(std::basic_string_view<Char>(str)));
    data_ = ss.data();
    size_ = ss.size();
  }

  constexpr value_type view() const noexcept
  {
    return value_type(data_, size_);
  }

  constexpr auto data() const noexcept
  {
    return data_;
  }

  constexpr auto size() const noexcept
  {
    return size_;
  }

  constexpr operator value_type() const noexcept
  {
    return view();
  }

  constexpr value_type operator*() const noexcept
  {
    return view();
  }

  constexpr bool operator==(basic_constant_string const& other) const noexcept
  {
    return view() == other.view();
  }
  constexpr auto operator<=>(basic_constant_string const& other) const noexcept
  {
    return view() <=> other.view();
  }
};

using constant_string = basic_constant_string<char>;

consteval auto operator""_sc(const char* data, std::size_t s) -> constant_string
{
  return {
    std::string_view{data, s}
  };
}

template <class Opts>
auto print_help(char** argv) -> void
{
  constexpr auto ctx = access_context::current();

  auto program_path = std::string(argv[0]);
  auto program_name = program_path.substr(program_path.find_last_of("/") + 1);

  std::cout << "Usage: " << program_name << " [options]\n";
  std::cout << "options:\n";

  // determine the longest option name (for alignment)
  std::size_t max_name_len = 0;
  template for (constexpr auto opt : std::define_static_array(nonstatic_data_members_of(^^Opts, ctx))) {
    std::string_view name = identifier_of(opt);
    if (name.size() > max_name_len) max_name_len = name.size();
  }

  const std::size_t desc_start = 4 + max_name_len + 2;

  template for (constexpr auto opt : std::define_static_array(nonstatic_data_members_of(^^Opts, ctx))) {
    std::string_view opt_name = identifier_of(opt);

    std::cout << "  --" << opt_name;

    std::size_t padding = desc_start - (4 + opt_name.size());
    std::cout << std::string(padding, ' ');

    bool first = true;
    template for (constexpr auto annot : std::define_static_array(annotations_of_with_type(opt, ^^constant_string))) {
      std::string_view desc = extract<constant_string>(annot).view();
      if (first) {
        std::cout << desc << '\n';
        first = false;
      } else {
        std::cout << std::string(desc_start, ' ') << desc << '\n';
      }
    }
  }

  std::exit(EXIT_SUCCESS);
}


template <class Opts>
auto cli_parse(int argc, char** argv, bool empty_opts_call_help = false) -> Opts {
  std::vector<std::string_view> cmdline(argv+1, argv+argc);
  constexpr auto ctx = access_context::current();

  if(empty_opts_call_help && cmdline.empty()) print_help<Opts>(argv);
  if(std::ranges::contains(cmdline, std::string_view("--help"))) print_help<Opts>(argv);

  Opts opts;

  template for (constexpr auto opt : std::define_static_array(nonstatic_data_members_of(^^Opts, ctx))) {

    auto it = std::ranges::find_if(cmdline,
      [=](std::string_view arg){
        return (arg.starts_with("--") && arg.substr(2) == identifier_of(opt));
      });

    if(it != cmdline.end())
    {
      if (it + 1 == cmdline.end() || (*(it+1)).starts_with("--")) {
        if constexpr (type_of(opt) == ^^bool){
          opts.[:opt:] = true;
          continue;
        } else {
          std::cerr << "Missing value for option " <<  display_string_of(opt) << std::endl;
          std::exit(EXIT_FAILURE);
        }
      }

      auto iss = std::ispanstream(*(it+1));
      if (iss >> opts.[:opt:]; !iss) {
        std::cerr << "Failed to parse `" << *(it+1) << "` to type " <<  display_string_of(type_of(opt)) << std::endl;
        std::exit(EXIT_FAILURE);
      }
    }
  }
  return opts;
}

struct Options {
  [[="name of ..."_sc]]
  std::string name;

  [[="count of ..."_sc]]
  [[="line 2 ..."_sc]]
  int count = 1;
};

int main(int argc, char** argv) {
  auto opts = cli_parse<Options>(argc, argv);

  std::cout << "opts.name : " << opts.name << std::endl;
  std::cout << "opts.count : " << opts.count << std::endl;
}