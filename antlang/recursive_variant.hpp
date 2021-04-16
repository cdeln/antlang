#pragma once

#include <memory>
#include <variant>
#include <type_traits>

namespace ant
{

template <typename T>
class recursive_wrapper
{
private:
    std::unique_ptr<T> value;
public:
    constexpr recursive_wrapper()
        : value{std::make_unique<T>()} {}

    constexpr recursive_wrapper(T const& value)
        : value{std::make_unique<T>(value)} {}

    constexpr recursive_wrapper(T&& value)
        : value{std::make_unique<T>(std::move(value))} {}

    constexpr recursive_wrapper(recursive_wrapper const& that)
        : recursive_wrapper(*that.value) {}

    constexpr recursive_wrapper(recursive_wrapper&& that)
        : recursive_wrapper(std::move(*that.value)) {}

    ~recursive_wrapper() = default;

    constexpr recursive_wrapper&
    operator=(T const& value) &
    {
        *this->value = value;
        return *this;
    }

    constexpr recursive_wrapper&
    operator=(T&& value) &
    {
        *this->value = std::move(value);
        return *this;
    }

    constexpr recursive_wrapper&
    operator=(recursive_wrapper const& that) &
    {
        *this->value = *that.value;
        return *this;
    }

    constexpr recursive_wrapper&
    operator=(recursive_wrapper&& that) &
    {
        this->value = std::move(that.value);
        return *this;
    }

    constexpr operator T& () &
    {
        return *value;
    }

    constexpr operator T const& () const&
    {
        return *value;
    }

    constexpr operator T&& () &&
    {
        return *value;
    }
};

template <typename T, typename U>
struct is_recursive
    : std::integral_constant<bool, false> {};

template <typename T, typename U>
struct is_recursive<T, recursive_wrapper<U>>
    : std::integral_constant<bool, std::is_same_v<T, U>> {};

template <typename T, typename... Ts>
struct is_recursive<T, std::variant<Ts...>>
    : std::integral_constant<bool, (is_recursive<T, Ts>::value || ...)> {};

template <typename T, typename U>
constexpr bool is_recursive_v = is_recursive<T, U>::value;

template <typename T, typename Variant>
constexpr bool holds(Variant const& variant) noexcept
{
    if constexpr (is_recursive<T, Variant>::value)
    {
        return std::holds_alternative<recursive_wrapper<T>>(variant);
    }
    else
    {
        return std::holds_alternative<T>(variant);
    }
}

template <typename T, typename Variant>
constexpr decltype(auto) get_recursive(Variant&& variant)
{
    if constexpr (is_recursive<T, std::remove_reference_t<std::remove_const_t<Variant>>>::value)
    {
        return static_cast<T>(std::get<recursive_wrapper<T>>(std::forward<Variant>(variant)));
    }
    else
    {
        return std::get<T>(std::forward<Variant>(variant));
    }
}

template <typename Visitor, typename Variant>
decltype(auto) visit(Visitor&& visitor, Variant&& variant)
{
    return std::visit(std::forward<Visitor>(visitor),
                      std::forward<Variant>(variant));
}

}  // namespace ant
