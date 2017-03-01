#pragma once

#ifndef _OT_MEMORY_H_
#define _OT_MEMORY_H_

#include <utility>
#include <openthread.h>

namespace ot
{
    // ot memory to be free (by ::otFreeMemory)
    template <typename T> class Memory
    {
    public:
        // resource type : otInstance, otDeviceList, otIp6ADdress,
        // uint8_t <::otGetExtendedAddress>, ...
        using R = T*;

        inline static constexpr R invalid() { return nullptr; }

        Memory(Memory&& rhs) noexcept :
            Memory{ std::move(rhs.res_) }
        {
        }

        explicit Memory(R&& res = invalid()) noexcept :
            res_{ std::move(res) }
        {
            res = invalid();
        }

        ~Memory()
        {
            close();
        }

        Memory& operator =(R&& res) noexcept
        {
            attach(std::move(res));
            return *this;
        }

        Memory& operator =(Memory&& rhs) noexcept
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
            if (res_ == invalid()) { return; }

#ifdef OTDLL
            ::otFreeMemory(res_);
#endif
            res_ = invalid();
        }

        void swap(Memory& rhs) noexcept
        {
            using std::swap;

            swap(res_, rhs.res_);
        }

    protected:
        Memory(const Memory&) = delete;
        Memory& operator =(const Memory&) = delete;

        R res_ = invalid();
    };
}

#endif // _OT_MEMORY_H_
