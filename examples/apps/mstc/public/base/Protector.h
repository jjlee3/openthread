#pragma once

#include <mutex>

namespace mstc
{
namespace base
{
template <class T>
class Protector : public T
{
public:
    using base_t = T;
    using base_t::base_t;

    template <typename F> auto mutexInvoke(F& f)
    {
        lock_t lock(m_);
        return f();
    }

    template <typename F> auto mutexInvoke(F& f) const
    {
        lock_t lock(m_);
        return f();
    }

    template <typename F> auto mutexInvoke(const F& f)
    {
        lock_t lock(m_);
        return f();
    }

    template <typename F> auto mutexInvoke(const F& f) const
    {
        lock_t lock(m_);
        return f();
    }

protected:
    using mutex_t = std::mutex;
    using lock_t  = std::lock_guard<mutex_t>;

    mutex_t& mutex() const { return m_; }

    mutable mutex_t m_;
}; // Monitor
} // namespace mstc
} // namespace base