#pragma once

#ifndef _OT_INTERFACE_H_
#define _OT_INTERFACE_H_

#include "openthread.h"
#include "OtDevice.h"

namespace ot
{
    class KDevice;

    class KInterface
    {
    public:
        explicit KInterface(const KDevice&) noexcept;
        KInterface(KInterface&& rhs) noexcept;
        ~KInterface();

        void close() noexcept;

    protected:
        KInterface(const KInterface&) = delete;
        KInterface& operator =(const KInterface&) = delete;

        // We could not assign ckInst_ part for these methods!
        KInterface& operator =(KInterface&& rhs) noexcept = delete;
        void swap(KInterface& rhs) noexcept = delete;

        const KDevice& ckDevice_;
        bool           up_        = false;
        ThreadError    threadErr_ = kThreadError_None;
    };
}

#endif // _OT_INTERFACE_H_