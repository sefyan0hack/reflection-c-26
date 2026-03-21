#include <string>
#include <string_view>
#include <source_location>
#include <array>
#include <utility>
#include <sstream>
#include <type_traits>
#include <format>
#include <meta>
#include <ranges>
#include <print>
#include <tuple>
#include <iostream>

using namespace std::meta;

template <info F, class R, class... Args>
    requires (is_function(F))
class FunctionImpl {
public:
    using FuncType = [:type_of(F):]*;
    using BeforeType = void(*)(void);
    using AfterType = void(*)(std::string);

    static constexpr auto Params = [:reflect_constant_array(parameters_of(F)):];


    FunctionImpl(FuncType fptr = &default_, BeforeType before = nullptr, AfterType after = nullptr)
        : m_Func(fptr)
        , m_Befor(before)
        , m_After(after)
        , m_Calls(0)
    {}

    auto operator()(Args... args, std::source_location l = std::source_location::current()) -> R
    {
        m_Calls++;

        if(m_Befor) [[unlikely]] m_Befor();

        if constexpr (std::is_void_v<R>) {
            m_Func(args...);
            if(m_After) [[likely]] m_After(function_info(l, args...));
        } else {
            R result = m_Func(args...);
            if(m_After) [[likely]] m_After(function_info(l, args...));
            return result;
        }
    }

    auto name() const -> std::string_view
    {
        return std::meta::identifier_of(F);
    }

    auto function() const -> FuncType
    {
        return m_Func;
    }

    auto return_type() const -> std::string_view
    {
        return std::meta::display_string_of(dealias(^^R));
    }

    auto args_types() const -> std::vector<std::string_view>
    {
        std::vector<std::string_view> result;
        template for (constexpr info p : [:reflect_constant_array(parameters_of(type_of(F))):]) {
            result.push_back(display_string_of(std::meta::dealias(p)));
        }
        return result;
    }

    auto calls() const -> std::size_t
    {
        return m_Calls;
    }

    static auto default_([[maybe_unused]] Args... args) -> R
    {
        if constexpr (!std::is_void_v<R>) return R{};
    }

private:
    auto this_func_sig(Args... args) const -> std::string
    {
        std::string result = std::format("{} {}(", return_type(), name());

        template for(constexpr auto i : std::views::indices(sizeof...(args))){
            if (i > 0) result += ", ";
            std::string v;
            auto val = args...[i];

            using T = std::decay_t<decltype(val)>;

            if constexpr (std::is_pointer_v<T>) {
                v = "utils::to_string(val)";
            } else if constexpr (std::is_arithmetic_v<T>) {
                v = std::to_string(val);
            } else if constexpr (std::is_convertible_v<T, std::string>) {
                v = std::string(val);
            } else if constexpr (requires(std::ostream& os) { os << val; }) {
                std::stringstream ss;
                ss << val;
                v = ss.str();
            } else {
                v = "??";
            }

            constexpr auto arg_name = identifier_of(Params[i]);

            result += std::format("{} {} = {}", args_types()[i], arg_name, v);
        }

        return result + ")";
    }

    auto function_info(std::source_location l, Args... args) -> std::string
    {
        return std::format(
            "call Number: {} ; instrments(Befor: {}, After: {}) ; `{}` -> [{}:{}]\n",
            m_Calls,
            m_Befor ? "true" : "false", m_After ? "true" : "false",
            this_func_sig(args...),
            l.file_name(), l.line()
        );
    }

private:
    FuncType m_Func;
    BeforeType m_Befor;
    AfterType m_After;
    std::size_t m_Calls;
};

consteval auto get_fn_args(info F) {
    std::vector args = { reflect_constant(F), return_type_of(F) };
    for (auto p : parameters_of(F)) {
        args.push_back(type_of(p));
    }
    return args;
}

template <info F>
using Function = [: substitute(^^FunctionImpl, get_fn_args(F)) :];

auto add(int a, int b) -> int { return a+b; }
int main(){

    auto p = +[](std::string i) -> void {
        std::cout << i << std::endl;
    };

    Function<^^add> f(&add, nullptr, p);

    std::cout << f.name() << std::endl;
    std::cout << f.function() << std::endl;
    std::cout << f.return_type() << std::endl;

    for(auto p : f.args_types())
        std::cout << p << std::endl;

    f(1,2);
}