#pragma once

#ifndef _OT_MESH_LOCAL_EID_H_
#define _OT_MESH_LOCAL_EID_H_

#include "openthread.h"
#include "OtMemory.h"
#include "OtFailure.h"

namespace ot
{
    class KDevice;

    class KMeshLocalEid : public KMemory<const otIp6Address>
    {
    public:
        class Exception : public KFailureException
        {
        public:
            using base_t = KFailureException;
            using base_t::base_t;
        };

    public:
        using base_t = KMemory<const otIp6Address>;
        // resourec type
        using R = base_t::R;
        using base_t::base_t;

        explicit KMeshLocalEid(const KDevice&);
        explicit KMeshLocalEid(otInstance*);
    };
}

#endif // _OT_MESH_LOCAL_EID_H_
