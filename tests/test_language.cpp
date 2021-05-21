#include <doctest/doctest.h>

#include "tokenize.hpp"
#include "parser.hpp"
#include "compiler.hpp"

using namespace ant;

namespace
{

std::pair<compiler_environment, runtime::program>
ensure_compiled(const std::string& source)
{
    const auto tokens = tokenize(source);
    const auto parser = make_parser<ast::program>();
    const auto parsed = parser.parse(tokens.cbegin(), tokens.cend());
    REQUIRE(is_success(parsed));
    const auto& statements = get_success(parsed).value;
    auto [env, prog] = setup_compiler();
    const auto compile_info = compile(prog, env, statements);
    for (auto const& status : compile_info)
    {
        REQUIRE(is_success(status));
    }
    return std::make_pair(std::move(env), std::move(prog));
}

} // namespace

TEST_CASE("factorial")
{
    const std::string source = R"(
        (function factorial u32 (u32 n)
          (when
            [(= n (u32 0)) (u32 1)]
            (* n (factorial (- n (u32 1))))
          )
        )
    )";
    auto [env, prog] = ensure_compiled(source);
    static_cast<void>(prog);
    auto query = find_function(env, "factorial", {"u32"});
    REQUIRE(is_success(query));
    REQUIRE(get_success(query).return_type == "u32");
    auto* func = get_success(query).function;
    REQUIRE(func->parameters.size() == 1);
    REQUIRE(holds<uint32_t>(func->parameters.at(0)));
    func->parameters.at(0) = uint32_t{10};
    auto result = execute(*func);
    REQUIRE(holds<uint32_t>(result));
    REQUIRE(get<uint32_t>(result) == 3628800);
}

TEST_CASE("fibonacci")
{
    const std::string source = R"(
        (function fibonacci u32 (u32 n)
          (when
            [(= n (u32 0)) (u32 1)]
            [(= n (u32 1)) (u32 1)]
            (let [x (fibonacci (- n (u32 1)))]
                 [y (fibonacci (- n (u32 2)))]
                 (+ x y)
            )
          )
        )
    )";
    auto [env, prog] = ensure_compiled(source);
    static_cast<void>(prog);
    auto query = find_function(env, "fibonacci", {"u32"});
    REQUIRE(is_success(query));
    REQUIRE(get_success(query).return_type == "u32");
    auto* func = get_success(query).function;
    REQUIRE(func->parameters.size() == 1);
    REQUIRE(holds<uint32_t>(func->parameters.at(0)));
    func->parameters.at(0) = uint32_t{10};
    auto result = execute(*func);
    REQUIRE(holds<uint32_t>(result));
    REQUIRE(get<uint32_t>(result) == 89);
}
