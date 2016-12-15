#pragma once

#ifndef _OT_EXT_ADDR_H_
#define _OT_EXT_ADDR_H_

#include "openthread.h"
#include "OtMemory.h"
#include "OtFailure.h"

namespace ot
{
    class KDevice;

    class KExtendedAddress : public KMemory<const uint8_t>
    {
    public:
        class Exception : public KFailureException
        {
        public:
            using base_t = KFailureException;
            using base_t::base_t;
        };

    public:
        using base_t = KMemory<const uint8_t>;
        // resourec type
        using R = base_t::R;
        using base_t::base_t;

        explicit KExtendedAddress(const KDevice&);
        explicit KExtendedAddress(otInstance*);
    };
}

#endif // _OT_DEVICE_H_