#pragma once
// refer to boost scoped_thread

#include <utility>
#include <thread>

namespace mstc
{
namespace base
{
struct join_if_joinable
{
    void operator ()(std::thread& t)
    {
        if (t.joinable()) { t.join(); }
    }
}; // join_if_joinable

template <class CallableThread = join_if_joinable>
class StrictScopedThreadC
{
public:
    template <class F, class ...Args>
    explicit StrictScopedThreadC(F&& f, Args&&... args) :
        t_{std::forward<F>(f), std::forward<Args>(args)...} {}

    StrictScopedThreadC(StrictScopedThreadC&& rhs) noexcept :
        t_{std::move(rhs.t_)} {}

    ~StrictScopedThreadC()
    {
        CallableThread onDestruct;
        onDestruct(t_);
    }

protected:
    std::thread t_;
}; // StrictScopedThreadC

using StrictScopedThread = StrictScopedThreadC<>;

template <class CallableThread = join_if_joinable>
class ScopedThreadC
{
public:
    ScopedThreadC() noexcept : t_{} {}

    template <class F, class ...Args>
    explicit ScopedThreadC(F&& f, Args&&... args) :
        t_{std::forward<F>(f), std::forward<Args>(args)...} {}

    ScopedThreadC(ScopedThreadC&& rhs) noexcept :
        t_{std::move(rhs.t_)} {}

    ~ScopedThreadC()
    {
        CallableThread onDestruct;
        onDestruct(t_);
    }

    ScopedThreadC& operator =(ScopedThreadC&& rhs) noexcept
    {
        t_ = std::move(rhs.t_);
        return *this;
    }

    void swap(ScopedThreadC& rhs) noexcept
    {
        t_.swap(rhs.t_);
    }

    auto get_id() const noexcept
    {
        return t_.get_id();
    }

    void detach()
    {
        t_.detach();
    }

    void join()
    {
        t_.join();
    }

    auto native_handle() noexcept
    {
        return t_.native_handle();
    }

    bool joinable() const noexcept
    {
        return t_.joinable();
    }

    static unsigned hardware_concurrency() noexcept
    {
        return std::thread::hardware_concurrency();
    }

protected:
    std::thread t_;
}; // ScopedThreadC

using ScopedThread = ScopedThreadC<>;

} // namespace mstc
} // namespace base