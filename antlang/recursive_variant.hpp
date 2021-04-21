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

    constexpr T& get() &
    {
        return *value;
    }

    constexpr T const& get() const&
    {
        return *value;
    }

    constexpr T&& get() &&
    {
        return std::move(*value);
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

template <typename... Ts>
class recursive_variant;

template <typename T, typename U>
struct is_recursive
    : std::integral_constant<bool, false> {};

template <typename T, typename U>
struct is_recursive<T, recursive_wrapper<U>>
    : std::integral_constant<bool, std::is_same_v<T, U>> {};

template <typename T, typename... Ts>
struct is_recursive<T, recursive_variant<Ts...>>
    : std::integral_constant<bool, (is_recursive<T, Ts>::value || ...)> {};

template <typename T, typename U>
constexpr bool is_recursive_v = is_recursive<T, U>::value;

template <typename T>
struct remove_qualifiers
{
    using type = std::remove_reference_t<std::remove_cv_t<T>>;
};

template <typename T>
using remove_qualifiers_t = typename remove_qualifiers<T>::type;

template <typename... Ts>
struct recursive_variant
{
    using storage_type = std::variant<Ts...>;

    storage_type storage;

    constexpr recursive_variant()
        noexcept(std::is_nothrow_default_constructible_v<storage_type>) = default;

    constexpr recursive_variant(recursive_variant const& that) = default;

    constexpr recursive_variant(recursive_variant&& that)
        noexcept(std::is_nothrow_move_constructible_v<storage_type>) = default;

    template <
        typename T,
        typename =
            std::enable_if_t<
                !std::is_base_of_v<
                    recursive_variant,
                    remove_qualifiers_t<T>
                 >
            >
    >
    constexpr recursive_variant(T&& x) noexcept(std::is_nothrow_constructible_v<storage_type, T>)
        : storage(std::forward<T>(x))
    {
    }

    constexpr recursive_variant& operator=(recursive_variant const& that) &
    {
        this->storage = that.storage;
        return *this;
    }

    constexpr recursive_variant& operator=(recursive_variant&& that) &
        noexcept(((std::is_nothrow_move_constructible_v<Ts> &&
                   std::is_nothrow_move_assignable_v<Ts>)   && ...))
    {
        this->storage = std::move(that.storage);
        return *this;
    }

    template <
        typename T,
        typename =
            std::enable_if_t<
                !std::is_base_of_v<
                    recursive_variant,
                    remove_qualifiers_t<T>
                 >
            >
    >
    constexpr recursive_variant& operator=(T&& value) &
        noexcept(((std::is_nothrow_assignable_v<Ts&, T>    &&
                   std::is_nothrow_constructible_v<Ts, T>) && ...))
    {
        this->storage = std::forward<T>(value);
        return *this;
    }

    virtual ~recursive_variant() = default;
};

template <typename T, typename... Ts>
constexpr bool holds(recursive_variant<Ts...> const& variant) noexcept
{
    if constexpr (is_recursive_v<T, recursive_variant<Ts...>>)
    {
        return std::holds_alternative<recursive_wrapper<T>>(variant.storage);
    }
    else
    {
        return std::holds_alternative<T>(variant.storage);
    }
}

template <typename T, typename... Ts>
constexpr T const& get(recursive_variant<Ts...> const& variant)
{
    if constexpr (is_recursive_v<T, recursive_variant<Ts...>>)
    {
        return std::get<recursive_wrapper<T>>(variant.storage);
    }
    else
    {
        return std::get<T>(variant.storage);
    }
}

template <typename T, typename... Ts>
constexpr T& get(recursive_variant<Ts...>& variant)
{
    if constexpr (is_recursive_v<T, recursive_variant<Ts...>>)
    {
        return std::get<recursive_wrapper<T>>(variant.storage);
    }
    else
    {
        return std::get<T>(variant.storage);
    }
}

template <typename T, typename... Ts>
constexpr T&& get(recursive_variant<Ts...>&& variant)
{
    if constexpr (is_recursive_v<T, recursive_variant<Ts...>>)
    {
        return std::get<recursive_wrapper<T>>(std::move(variant).storage);
    }
    else
    {
        return std::get<T>(std::move(variant).storage);
    }
}

template <typename Visitor, typename Variant>
struct recursive_visitor
{
    Visitor visitor;

    constexpr recursive_visitor(Visitor visitor)
        noexcept(std::is_nothrow_move_constructible_v<Visitor>)
        : visitor(std::move(visitor)) {}

    template <typename T>
    constexpr decltype(auto) operator()(T&& x)
    {
        if constexpr (is_recursive_v<remove_qualifiers_t<T>, Variant>)
        {
            return visitor(std::forward<T>(x).get());
        }
        else
        {
            return visitor(std::forward<T>(x));
        }
    }
};

template <typename Visitor, typename Variant>
decltype(auto) visit(Visitor&& visitor, Variant&& variant)
{
    using visitor_type = remove_qualifiers_t<Visitor>;
    using variant_type = remove_qualifiers_t<Variant>;
    return std::visit(recursive_visitor<visitor_type, variant_type>(std::forward<Visitor>(visitor)),
                      std::forward<Variant>(variant).storage);
}

}  // namespace ant
