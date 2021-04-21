#pragma once

#include "recursive_variant.hpp"

namespace ant
{

template <typename Success, typename Failure>
struct exceptional
{
    recursive_variant<Success, Failure> value;

    constexpr exceptional() = default;

    exceptional(Success const& alternative)
        : value(alternative) {}

    exceptional(Success&& alternative)
        : value(std::move(alternative)) {}

    exceptional(Failure const& alternative)
        : value(std::move(alternative)) {}

    exceptional(Failure&& alternative)
        : value(std::move(alternative)) {}
};

template <typename Success, typename Failure>
bool is_success(exceptional<Success, Failure> const& result)
{
    return holds<Success>(result.value);
}

template <typename Success, typename Failure>
bool is_failure(exceptional<Success, Failure> const& result)
{
    return holds<Failure>(result.value);
}

template <typename Success, typename Failure>
Success&
get_success(exceptional<Success, Failure>& result)
{
    return get<Success>(result.value);
}

template <typename Success, typename Failure>
Success const&
get_success(exceptional<Success, Failure> const& result)
{
    return get<Success>(result.value);
}

template <typename Success, typename Failure>
Failure&
get_failure(exceptional<Success, Failure>& result)
{
    return get<Failure>(result.value);
}

template <typename Success, typename Failure>
Failure const&
get_failure(exceptional<Success, Failure> const& result)
{
    return get<Failure>(result.value);
}

}  // namespace ant
