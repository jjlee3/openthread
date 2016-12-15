#pragma once

#include <utility>

namespace mstc
{
namespace base
{
template <typename F>
class Scope
{
public:
    explicit Scope(F&& f) noexcept : f_{std::move(f)} {}

    ~Scope() { f_(); }

protected:
    F f_;
}; // Scope

// factory function
template <typename F> Scope<F> scope(F&& f) noexcept
{
    return Scope<F>{std::move(f)};
}

} // namespace mstc
} // namespace base