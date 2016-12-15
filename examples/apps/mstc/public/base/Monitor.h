#pragma once
// refer to Herb Sutter in C++ and Beyond 2012

#include <mutex>

namespace mstc
{
namespace base
{
template <class T>
class Monitor
{
public:
    explicit Monitor(T t = T{}) : t_{t} {}

    template <typename F> auto operator ()(F& f)
    {
        lock_t lock(m_);
        return f(t_);
    }

    template <typename F> auto operator ()(F& f) const
    {
        lock_t lock(m_);
        return f(t_);
    }

    template <typename F> auto operator ()(const F& f)
    {
        lock_t lock(m_);
        return f(t_);
    }

    template <typename F> auto operator ()(const F& f) const
    {
        lock_t lock(m_);
        return f(t_);
    }

protected:
    using mutex_t = std::mutex;
    using lock_t  = std::lock_guard<mutex_t>;

    mutex_t& mutex() const { return m_; }

    T               t_;
    mutable mutex_t m_;
}; // Monitor
} // namespace mstc
} // namespace base