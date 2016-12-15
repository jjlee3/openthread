#pragma once

#include <utility>

namespace mstc
{
namespace ot
{
template <typename T> class KMemory
{
public:
    // resourec type
    using R = T*;

    inline static constexpr R invalid() { return nullptr; }

    KMemory(KMemory&& rhs) noexcept :
        KMemory{std::move(rhs.res_)}
    {
    }

    explicit KMemory(R&& res = invalid()) noexcept :
        res_{std::move(res)}
    {
        res = invalid();
    }

    ~KMemory()
    {
        close();
    }

    KMemory& operator =(R&& res) noexcept
    {
        attach(std::move(res));
        return *this;
    }

    KMemory& operator =(KMemory&& rhs) noexcept
    {
        if (this == &rhs) { return *this; }

        *this = std::move(rhs.res_);
        return *this;
    }

    R get() const noexcept
    {
        return res_;
    }

    void attach(R&& res) noexcept
    {
        close();
        res_ = res;
        res = invalid();
        return;
    }

    R detach() noexcept
    {
        auto res = res_;
        res_ = invalid();
        return res;
    }

    void close()
    {
        if (!res_) { return; }

#ifdef OTDLL
        ::otFreeMemory(res_);
#endif
        res_ = invalid();
    }

    void swap(KMemory& rhs) noexcept
    {
        using std::swap;

        swap(res_, rhs.res_);
    }

protected:
    KMemory(const KMemory&) = delete;
    KMemory& operator =(const KMemory&) = delete;

    R res_ = invalid();
}; // KMemory
} // ot
} // mstc